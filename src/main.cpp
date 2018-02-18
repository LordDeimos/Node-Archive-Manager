#include <iostream>
#include <nan.h>
#include <sstream>
#include <stdexcept>
#include <string.h>

#include <archive.h>
#include <archive_entry.h>

#define BLOCK_SIZE 10240

namespace archive_manager {

using namespace v8;
using namespace Nan;

typedef struct archive* archive_t;
typedef struct archive_entry* archive_entry_t;

#pragma region //Helpers

struct metadata_t {
    const char* name;
    int size;
    bool isDir;
};

std::vector<std::string> split(const char* string, char delim) {
    std::vector<std::string> tokens;
    std::string input(string);
    std::istringstream ss(input);
    std::string token;

    while (std::getline(ss, token, delim)) {
        tokens.push_back(token);
    }
    return tokens;
}

void set_filter(archive_t archive, const char* file) {
    std::vector<std::string> tokens = split(file, '.');
    if (tokens.size() >= 2) {
        if (!tokens.back().compare("7zip") || !tokens.back().compare("cb7")) {
            archive_write_set_format_7zip(archive);
            archive_write_add_filter_none(archive);
        } else if (!tokens.back().compare("zip") || !tokens.back().compare("cbz")) {
            archive_write_set_format_zip(archive);
            archive_write_add_filter_none(archive);
        } else if (!tokens.back().compare("jar")) {
            archive_write_set_format_zip(archive);
            archive_write_add_filter_none(archive);
        } else if (!tokens.back().compare("cpio")) {
            archive_write_set_format_cpio(archive);
            archive_write_add_filter_none(archive);
        } else if (!tokens.back().compare("iso")) {
            archive_write_set_format_iso9660(archive);
            archive_write_add_filter_none(archive);
        } else if (!tokens.back().compare("tar")) {
            archive_write_set_format_pax_restricted(archive);
            archive_write_add_filter_none(archive);
        } else if (!tokens.back().compare("tgz")) {
            archive_write_set_format_pax_restricted(archive);
            archive_write_add_filter_gzip(archive);
        } else if (!tokens.back().compare("gz")) {
            if (tokens[tokens.size() - 2] == "tar") {
                archive_write_set_format_pax_restricted(archive);
                archive_write_add_filter_gzip(archive);
            }
        } else if (!tokens.back().compare("bz2")) {
            if (tokens[tokens.size() - 2] == "tar") {
                archive_write_set_format_pax_restricted(archive);
                archive_write_add_filter_bzip2(archive);
            }
        } else if (!tokens.back().compare("xz")) {
            if (tokens[tokens.size() - 2] == "tar") {
                archive_write_set_format_pax_restricted(archive);
                archive_write_add_filter_xz(archive);
            }
        } else if (!tokens.back().compare("a") || !tokens.back().compare("ar")) {
#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__NetBSD__) || defined(__OpenBSD__)
            archive_write_set_format_ar_bsd(archive);
            archive_write_add_filter_none(archive);
#else
            archive_write_set_format_ar_svr4(archive);
            archive_write_add_filter_none(archive);
#endif
        } else {
            archive_write_set_format_zip(archive);
            archive_write_add_filter_none(archive);
        }
    }
}

std::vector<char> cat(std::vector<char> left, char* right, int rightSize) {
    std::vector<char> output;
    for (int i = 0; i < left.size(); i++) {
        output.push_back(left[i]);
    }
    for (int i = 0; i < rightSize; i++) {
        output.push_back(right[i]);
    }
    return output;
}

#pragma endregion

#pragma region //Un-Wrapped

std::vector<std::string> view(const char* file) {
    archive_t archive;
    archive_entry_t entry;
    int r;

    archive = archive_read_new();
    archive_read_support_filter_all(archive);
    archive_read_support_format_all(archive);

    std::vector<std::string> array;

    r = archive_read_open_filename(archive, file, 10240);
    if (r != ARCHIVE_OK) {
        throw std::runtime_error(archive_error_string(archive));
        return std::vector<std::string>();
    }
    int i = 0;
    while (archive_read_next_header(archive, &entry) != ARCHIVE_EOF) {
        array.push_back(std::string(archive_entry_pathname(entry)));
        archive_read_data_skip(archive);
        i++;
    }
    r = archive_read_free(archive);

    if (r != ARCHIVE_OK) {
        throw std::runtime_error(archive_error_string(archive));
        return std::vector<std::string>();
    }
    return array;
}

metadata_t* getinfo(const char* fileName, const char* archivePath) {
    archive_t archive;
    archive_entry_t entry;
    int r;
    metadata_t* object = (metadata_t*)malloc(sizeof(metadata_t));

    archive = archive_read_new();
    archive_read_support_filter_all(archive);
    archive_read_support_format_all(archive);

    r = archive_read_open_filename(archive, archivePath, BLOCK_SIZE);
    if (r != ARCHIVE_OK) {
        throw std::runtime_error(archive_error_string(archive));
        return NULL;
    }

    while (archive_read_next_header(archive, &entry) == ARCHIVE_OK) {
        if (!strcmp(archive_entry_pathname(entry), fileName)) {
            object->name = archive_entry_pathname(entry);
            object->size = archive_entry_size(entry);
            object->isDir = (archive_entry_filetype(entry) == AE_IFDIR);
            //more?
            break;
        }
    }

    r = archive_read_free(archive);

    if (r != ARCHIVE_OK) {
        throw std::runtime_error(archive_error_string(archive));
        return NULL;
    }
    return object;
}

bool writeLocal(std::vector<std::string> files, std::string archivePath) {
    //This might take some doing
    archive_t archive;
    archive_entry_t entry;
    struct stat st;
    char buff[BLOCK_SIZE];
    int len;
    FILE* fd = NULL;

    archive = archive_write_new();

    //archive_write_set_format_filter_by_ext(archive,*file); only for libarchive >=3.2
    set_filter(archive, archivePath.c_str());

    if (archive_write_open_filename(archive, archivePath.c_str()) != ARCHIVE_OK) {
        throw std::runtime_error(archive_error_string(archive));
        return false;
    }
    for (int i = 0; i < files.size(); i++) {
        std::vector<std::string> path = split(files[i].c_str(), '/');

        const char* internalName = path.back().c_str();
        stat(files[i].c_str(), &st);
        entry = archive_entry_new();

        archive_entry_set_pathname(entry, internalName);
        archive_entry_set_size(entry, st.st_size);
        archive_entry_set_filetype(entry, AE_IFREG);
        archive_entry_set_perm(entry, 0644);
        archive_write_header(archive, entry);
        fd = fopen(files[i].c_str(), "r");
        if (fd) {
            len = fread(buff, sizeof(buff), sizeof(char), fd);
            while (len > 0) {
                archive_write_data(archive, buff, len);
                len = fread(buff, sizeof(buff), sizeof(char), fd);
            }
            fclose(fd);
        }
        archive_entry_free(entry);
    }
    if (archive_write_close(archive) != ARCHIVE_OK) {
        throw std::runtime_error(archive_error_string(archive));
        return false;
    }
    archive_write_close(archive);
    archive_write_free(archive);

    return true;
}

bool extract(std::string archivePath, std::string outputPath) {
    archive_t archive, archivew;
    archive_entry_t entry;
    int response, flags = ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_ACL | ARCHIVE_EXTRACT_FFLAGS;

    archive = archive_read_new();
    archive_read_support_format_all(archive);
    archive_read_support_compression_all(archive);
    archivew = archive_write_disk_new();
    archive_write_disk_set_options(archivew, flags);
    archive_write_disk_set_standard_lookup(archivew);

    if ((response = archive_read_open_filename(archive, archivePath.c_str(), BLOCK_SIZE))) {
        throw std::runtime_error(archive_error_string(archive));
        return false;
    }

    while ((response = archive_read_next_header(archive, &entry)) != ARCHIVE_EOF) {
        if (response < ARCHIVE_WARN) {
            throw std::runtime_error(archive_error_string(archive));
            return false;
        }
        std::string internal(archive_entry_pathname(entry));
        std::string path(outputPath.c_str());
        archive_entry_set_pathname(entry, (path + internal).c_str());
        if ((response = archive_write_header(archivew, entry)) != ARCHIVE_OK) {
            throw std::runtime_error(archive_error_string(archive));
            return false;
        } else if (archive_entry_size(entry) > 0) {
            const void* buffer;
            size_t size;
            int64_t offset;
            while ((response = archive_read_data_block(archive, &buffer, &size, &offset)) != ARCHIVE_EOF) {
                if (response < ARCHIVE_OK) {
                    throw std::runtime_error(archive_error_string(archive));
                    return false;
                }
                if ((response = archive_write_data_block(archivew, buffer, size, offset)) < ARCHIVE_OK) {
                    throw std::runtime_error(archive_error_string(archive));
                    return false;
                }
            }
        }
    }
    response = archive_read_free(archive);
	if(response!=ARCHIVE_OK){
		throw std::runtime_error(archive_error_string(archive));
		return false;
	}
    response = archive_write_free(archivew);
	if(response!=ARCHIVE_OK){
		throw std::runtime_error(archive_error_string(archivew));
		return false;
	}

    return true;
}

bool appendLocal(Local<Array> newFiles, Local<String> archivePath) {
    /**
   * Because there is no way to do in-place edits:
   * extract to temp dir
   * create list of old files and new files
   * write all files over the old archive
  */

    std::string tempDir("./tmp/");
    String::Utf8Value path(archivePath);
    std::vector<std::string> content = view(*path);
    Local<Array> oldFiles = Nan::New<Array>(content.size());
    for (int i = 0; i < content.size(); i++) {
        Nan::Set(oldFiles, i, Nan::New<String>(content[i].c_str()).ToLocalChecked());
    }

    for (int i = 0; i < oldFiles->Length(); i++) {
        String::Utf8Value temp(oldFiles->Get(i)->ToString());
        std::string path(*temp);
        Nan::Set(newFiles, newFiles->Length(), Nan::New<String>(tempDir + path).ToLocalChecked());
    }

    /*if (extract(archivePath, Nan::New<String>(tempDir).ToLocalChecked())) {
        return true; //writeLocal(newFiles, archivePath);
    }*/

    return false;
}

std::vector<char> getData(std::string internalPath, std::string archivePath) {
    archive_t archive;
    archive_entry_t entry;
    int response;
    std::vector<char> output;
    size_t totalsize = 0;

    archive = archive_read_new();
    archive_read_support_format_all(archive);
    archive_read_support_compression_all(archive);
    if ((response = archive_read_open_filename(archive, archivePath.c_str(), BLOCK_SIZE))) {
        throw std::runtime_error(archive_error_string(archive));
        return std::vector<char>();
    }

    while ((response = archive_read_next_header(archive, &entry)) != ARCHIVE_EOF) {
        if (response < ARCHIVE_WARN) {
            throw std::runtime_error(archive_error_string(archive));
            return std::vector<char>();
        } else if (archive_entry_size(entry) > 0) {
            if (!strcmp(archive_entry_pathname(entry), internalPath.c_str())) {
                const void* buffer;
                size_t size;
                int64_t offset;
                while ((response = archive_read_data_block(archive, &buffer, &size, &offset)) != ARCHIVE_EOF) {
                    if (response < ARCHIVE_OK) {
                        throw std::runtime_error(archive_error_string(archive));
                        return std::vector<char>();
                    }
                    totalsize += size;
                    if (size > 0) {
                        output = cat(output, (char*)buffer, size);
                    }
                }
                break;
            }
        }
    }
    archive_read_free(archive);
    return output;
}

    /*Local<Boolean> writeMemory(Local<String> file, Local<String> archivePath){
  //This might take some more doing
  return Nan::False();
}*/

    /**
 * ToDo
 * - Remove file/folder from archive
 * - Append/Write from in memory
*/

#pragma endregion

#pragma region //Worker Classes

class ViewWorker : public Nan::AsyncWorker {
  private:
    std::string path;
    std::vector<std::string> files;

  public:
    ViewWorker(Nan::Callback* callback, std::string path)
        : AsyncWorker(callback) {
        this->path = path;
        this->files = std::vector<std::string>();
    }

    void Execute() {
        try {
            files = view(path.c_str());
        } catch (const char* e) {
            this->SetErrorMessage(e);
        }
    }

    void HandleOKCallback() {
        Nan::HandleScope scope;
        Local<Array> output = Nan::New<Array>(files.size());
        for (int i = 0; i < files.size(); i++) {
            Nan::Set(output, i, Nan::New<String>(files[i].c_str()).ToLocalChecked());
        }
        Local<Value> argv[] = {Nan::Null(), output};
        callback->Call(2, argv);
    }
    void HandleErrorCallback() {
        Nan::HandleScope scope;
        Local<Value> argv[] = {Nan::New<String>(this->ErrorMessage()).ToLocalChecked(), Nan::Null()};
        callback->Call(2, argv);
    }
};

class InfoWorker : public Nan::AsyncWorker {
  private:
    std::string archivePath, internalPath;
    metadata_t* data;

  public:
    InfoWorker(Callback* callback, std::string internalPath, std::string archivePath)
        : AsyncWorker(callback) {
        this->archivePath = archivePath;
        this->internalPath = internalPath;
    }

    void Execute() {
        try {
            data = getinfo(internalPath.c_str(), archivePath.c_str());
        } catch (const char* e) {
            this->SetErrorMessage(e);
        }
    }

    void HandleOKCallback() {
        Nan::HandleScope scope;
        Local<Object> output = Nan::New<Object>();
        Nan::Set(output, New<String>("name").ToLocalChecked(), New<String>(data->name).ToLocalChecked());
        Nan::Set(output, New<String>("size").ToLocalChecked(), New<Number>(data->size));
        Nan::Set(output, New<String>("directory").ToLocalChecked(), New<Boolean>(data->isDir));
        Local<Value> argv[] = {
            Nan::Null(),
            output};
        callback->Call(2, argv);
    }

    void HandleErrorCallback() {
        Nan::HandleScope scope;
        Local<Value> argv[] = {
            Nan::New<String>(this->ErrorMessage()).ToLocalChecked(),
            Nan::Null()};
        callback->Call(2, argv);
    }
};

class WriteWorker : public Nan::AsyncWorker {
  private:
    std::vector<std::string> files;
    std::string archivePath;
    bool outcome;

  public:
    WriteWorker(Callback* callback, std::vector<std::string> files, std::string archivePath)
        : AsyncWorker(callback) {
        this->files = files;
        this->archivePath = archivePath;
    }

    void Execute() {
        try {
            outcome = writeLocal(files, archivePath);
        } catch (std::exception& e) {
            this->SetErrorMessage(e.what());
        }
    }

    void HandleOKCallback() {
        Nan::HandleScope scope;
        Local<Value> argv[] = {
            Nan::Null(),
            Nan::New<Boolean>(outcome)};
        callback->Call(2, argv);
    }

    void HandleErrorCallback() {
        Nan::HandleScope scope;
        Local<Value> argv[] = {
            Nan::New<String>(this->ErrorMessage()).ToLocalChecked(),
            Nan::Null()};
        callback->Call(2, argv);
    }
};

class ExtractWorker : public Nan::AsyncWorker {
  private:
    std::string archivePath, outputPath;
    bool outcome;

  public:
    ExtractWorker(Callback* callback, std::string archivePath, std::string outputPath) : AsyncWorker(callback) {
        this->archivePath = archivePath;
        this->outputPath = outputPath;
    }

    void Execute() {
        try {
            outcome = extract(archivePath, outputPath);
        } catch (std::exception& e) {
            this->SetErrorMessage(e.what());
        }
    }

	void HandleOKCallback(){
		Nan::HandleScope scope;
        Local<Value> argv[] = {
            Nan::Null(),
            Nan::New<Boolean>(outcome)};
        callback->Call(2, argv);
	}

	void HandleErrorCallback(){
        Nan::HandleScope scope;
        Local<Value> argv[] = {
            Nan::New<String>(this->ErrorMessage()).ToLocalChecked(),
            Nan::Null()};
        callback->Call(2, argv);
	}
};

class ReadWorker : public Nan::AsyncWorker {
	private:
	std::string internalPath, archivePath;
	std::vector<char> output;

	public:
	ReadWorker(Callback* callback, std::string internalPath, std::string archivePath) : AsyncWorker(callback){
		this->internalPath = internalPath;
		this->archivePath = archivePath;
	}

	void Execute(){
		try{
			output = getData(internalPath,archivePath);
		}
		catch(std::exception& e){
			this->SetErrorMessage(e.what());
		}
	}

	void HandleOKCallback(){
		Nan::HandleScope scope;
		if(output.size()){
			Local<Value> argv[] = {
				Nan::Null(),
				Nan::CopyBuffer(output.data(), output.size()).ToLocalChecked()
			};
        	callback->Call(2, argv);
		}
		else{
			Local<Value> argv[] = {
				Nan::Null(),
				Nan::Undefined()
			};
        	callback->Call(2, argv);
		}
	}

	void HandleErrorCallback(){
        Nan::HandleScope scope;
        Local<Value> argv[] = {
            Nan::New<String>(this->ErrorMessage()).ToLocalChecked(),
            Nan::Null()};
        callback->Call(2, argv);
	}
};

#pragma endregion

#pragma region //Wrappers

NAN_METHOD(ListContent) {
    if (info.Length() != 2) {
        Nan::ThrowError("This Takes Two Arguments");
        return;
    }
    if (!info[0]->IsString()) {
        Nan::ThrowError("Expected Path as First Arg");
        return;
    }
    if (!info[1]->IsFunction()) {
        Nan::ThrowError("Expected Callback as Second Arg");
        return;
    }
    String::Utf8Value utf8path(info[0]->ToString());
    std::string path(*utf8path);
    Callback* callback = new Callback(Nan::To<Function>(info[1]).ToLocalChecked());
    Nan::AsyncQueueWorker(new ViewWorker(callback, path));
}

NAN_METHOD(GetInfo) {
    if (info.Length() != 3) {
        Nan::ThrowError("Usage: GetInfo(fileName, archivePath, callback)");
        return;
    }
    if (!info[0]->IsString()) {
        Nan::ThrowError("fileName Must be a string.");
        return;
    }
    if (!info[1]->IsString()) {
        Nan::ThrowError("archivePath Must be a string.");
        return;
    }
    if (!info[2]->IsFunction()) {
        Nan::ThrowError("callback Must be a function.");
        return;
    }
    String::Utf8Value utf8internal(info[0]->ToString()), utf8archive(info[1]->ToString());
    std::string internalPath(*utf8internal), archivePath(*utf8archive);
    Callback* callback = new Callback(Nan::To<Function>(info[2]).ToLocalChecked());
    Nan::AsyncQueueWorker(new InfoWorker(callback, internalPath, archivePath));
}

NAN_METHOD(WriteFromDisk) {
    if (info.Length() == 3) {
        if (!info[0]->IsArray()) {
            Nan::ThrowError("newFiles Must be Array");
            return;
        }
        Callback* callback = new Callback(Nan::To<Function>(info[2]).ToLocalChecked());

        std::vector<std::string> files;
        for (int i = 0; i < Local<Array>::Cast(info[0])->Length(); i++) {
            String::Utf8Value tmp(Local<Array>::Cast(info[0])->Get(i));
            std::string entry(*tmp);
            files.push_back(entry);
        }
        String::Utf8Value val(info[1]->ToString());
        std::string archivePath(*val);
        Nan::AsyncQueueWorker(new WriteWorker(callback, files, archivePath));
    } else {
        Nan::ThrowError("Usage: WriteFromDisk([newFiles], archivePath, callback)");
    }
}

NAN_METHOD(Append) {
    if (info.Length() == 2) {
        if (!info[0]->IsArray()) {
            Nan::ThrowError("newFiles Must be Array");
            return;
        }
        if (!info[1]->IsString()) {
            Nan::ThrowError("archivePath Must be string");
            return;
        }
        info.GetReturnValue().Set(appendLocal(Local<Array>::Cast(info[0]), info[1]->ToString()));
    } else {
        Nan::ThrowError("Usage: Append([newFiles], archivePath, callback)");
    }
}

NAN_METHOD(Extract) {
    if (info.Length() == 2) {
		if(!info[0]->IsString()){
			Nan::ThrowError("archivePath Must be string");
			return;
		}
		if(!info[1]->IsFunction()){
			Nan::ThrowError("callback Must be function");
			return;
		}

		String::Utf8Value ap(info[0]->ToString());
		Callback* callback = new Callback(Nan::To<Function>(info[1]).ToLocalChecked());
		Nan::AsyncQueueWorker(new ExtractWorker(callback,std::string(*ap),std::string("./")));
    } else if (info.Length() == 3) {		
		if(!info[0]->IsString()){
			Nan::ThrowError("archivePath Must be string");
			return;
		}
		if(!info[1]->IsString()){
			Nan::ThrowError("outputPath Must be string");
			return;
		}
		if(!info[2]->IsFunction()){
			Nan::ThrowError("callback Must be function");
			return;
		}
		String::Utf8Value ap(info[0]->ToString());
		String::Utf8Value op(info[1]->ToString());
		Callback* callback = new Callback(Nan::To<Function>(info[2]).ToLocalChecked());
		Nan::AsyncQueueWorker(new ExtractWorker(callback,std::string(*ap),std::string(*op)));
    } else {
        Nan::ThrowError("Usage: Extract(archivePath, optional: outputPath, callback)");
    }
}

NAN_METHOD(ReadBuffer) {
    if (info.Length() == 3) {		
        if (!info[0]->IsString()) {
            Nan::ThrowError("internalPath Must be string");
            return;
        }		
        if (!info[1]->IsString()) {
            Nan::ThrowError("archivePath Must be string");
            return;
        }
        if (!info[2]->IsFunction()) {
            Nan::ThrowError("callback Must be function");
            return;
        }
		Callback* callback = new Callback(Nan::To<Function>(info[2]).ToLocalChecked());
		Nan::AsyncQueueWorker(new ReadWorker(callback,std::string(*String::Utf8Value(info[0]->ToString())),std::string(*String::Utf8Value(info[1]->ToString()))));
    } else {
        Nan::ThrowError("Usage: ReadBuffer(internalPath, archivePath, callback)");
    }
}

#pragma endregion

#pragma region //Node

NAN_MODULE_INIT(init) {
    Nan::Set(target, New<String>("ListContent").ToLocalChecked(),
             GetFunction(New<FunctionTemplate>(ListContent)).ToLocalChecked());

    Nan::Set(target, New<String>("GetInfo").ToLocalChecked(),
             GetFunction(New<FunctionTemplate>(GetInfo)).ToLocalChecked());

    Nan::Set(target, New<String>("WriteFromDisk").ToLocalChecked(),
             GetFunction(New<FunctionTemplate>(WriteFromDisk)).ToLocalChecked());

    Nan::Set(target, New<String>("Append").ToLocalChecked(),
             GetFunction(New<FunctionTemplate>(Append)).ToLocalChecked());

    Nan::Set(target, New<String>("Extract").ToLocalChecked(),
             GetFunction(New<FunctionTemplate>(Extract)).ToLocalChecked());

    Nan::Set(target, New<String>("ReadBuffer").ToLocalChecked(),
             GetFunction(New<FunctionTemplate>(ReadBuffer)).ToLocalChecked());
}

NODE_MODULE(manager, init);

#pragma endregion

} // namespace archive_manager