#include <fstream>
#include <iostream>
#include <nan.h>
#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined _WIN32
#include <archive.hpp>
#include <archive_entry.hpp>
#include <windows.h>

#else
#include <archive.h>
#include <archive_entry.h>
#endif

#define BLOCK_SIZE 10240

namespace archive_manager {

using namespace v8;
using namespace Nan;

typedef struct archive* archive_t;
typedef struct archive_entry* archive_entry_t;

#pragma region //Helpers

struct metadata_t {
    std::string name;
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

#if defined _WIN32
void removeDirWin(const char* dir) {
    WIN32_FIND_DATAA fileData;
    HANDLE handle = INVALID_HANDLE_VALUE;
    char path[BLOCK_SIZE];
    strcat(path, dir);
    strcat(path, "\\*.*");
    if ((handle = FindFirstFileA(path, &fileData)) == INVALID_HANDLE_VALUE) {
        throw std::runtime_error(std::string("Cannot open directory: ") + std::string(path));
        return;
    }

    do {
        if (strcmp(fileData.cFileName, ".") != 0 && strcmp(fileData.cFileName, "..") != 0) {
            if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                removeDirWin(fileData.cFileName);
            } else {
                remove((std::string(dir) + std::string(fileData.cFileName)).c_str());
            }
        }
    } while (FindNextFileA(handle, &fileData));
    FindClose(handle);
    RemoveDirectoryA(dir);
}
#endif

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

std::vector<metadata_t> view(std::string file) {
    archive_t archive;
    archive_entry_t entry;
    int r;

    archive = archive_read_new();
    archive_read_support_filter_all(archive);
    archive_read_support_format_all(archive);

    std::vector<metadata_t> array;

    r = archive_read_open_filename(archive, file.c_str(), 10240);
    if (r != ARCHIVE_OK) {
        throw std::runtime_error(archive_error_string(archive));
        return std::vector<metadata_t>();
    }
    int i = 0;
    while (archive_read_next_header(archive, &entry) != ARCHIVE_EOF) {
        metadata_t* object = new metadata_t;
        object->name = std::string(archive_entry_pathname(entry));
        object->size = archive_entry_size(entry);
        object->isDir = (archive_entry_filetype(entry) == AE_IFDIR);
        array.push_back(*object);
        archive_read_data_skip(archive);
        i++;
    }
    r = archive_read_free(archive);

    if (r != ARCHIVE_OK) {
        throw std::runtime_error(archive_error_string(archive));
        return std::vector<metadata_t>();
    }
    return array;
}

bool writeLocal(std::vector<std::string> files, std::string archivePath) {
    archive_t archive;
    archive_entry_t entry;
    struct stat st;
    char* buff;
    int len;

    archive = archive_write_new();

    //archive_write_set_format_filter_by_ext(archive,archivePath.c_str());// only for libarchive >=3.2
    set_filter(archive, archivePath.c_str());

    if (archive_write_open_filename(archive, archivePath.c_str()) != ARCHIVE_OK) {
        throw std::runtime_error(archive_error_string(archive));
        return false;
    }
    std::ifstream is;
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

        is.open(files[i].c_str(), std::ifstream::binary);
        if (is.is_open()) {
            is.seekg(0, is.end);
            len = is.tellg();
            is.seekg(0, is.beg);

            buff = new char[len];
            is.read(buff, len);
            archive_write_data(archive, buff, len);

            is.close();
            archive_entry_free(entry);
        } else {
            is.close();
            archive_entry_free(entry);
            throw std::runtime_error("Falied to open: " + files[i]);
            return false;
        }
    }
    if (archive_write_close(archive) != ARCHIVE_OK) {
        throw std::runtime_error(archive_error_string(archive));
        return false;
    }
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
    if (response != ARCHIVE_OK) {
        throw std::runtime_error(archive_error_string(archive));
        return false;
    }
    response = archive_write_free(archivew);
    if (response != ARCHIVE_OK) {
        throw std::runtime_error(archive_error_string(archivew));
        return false;
    }

    return true;
}

bool appendLocal(std::vector<std::string> newFiles, std::string archivePath) {
    std::string tempDir("./tmp/");
    std::vector<metadata_t> content = view(archivePath.c_str());

    for (int i = 0; i < content.size(); i++) {
        newFiles.push_back(tempDir + content[i].name);
    }

    if (extract(archivePath, tempDir)) {
        if (writeLocal(newFiles, archivePath)) {
#if defined _WIN32
            removeDirWin(tempDir.c_str());
#else
            system((std::string("rm -rf ") + tempDir).c_str());
#endif
            return true;
        }
    }

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

bool writeBuffer(std::vector<std::string> fileNames, std::vector<const char*> fileData, std::vector<size_t> fileSizes, std::string archivePath) {
    archive_t archive;
    archive_entry_t entry;

    archive = archive_write_new();

    //archive_write_set_format_filter_by_ext(archive,archivePath.c_str());// only for libarchive >=3.2
    set_filter(archive, archivePath.c_str());

    if (archive_write_open_filename(archive, archivePath.c_str()) != ARCHIVE_OK) {
        throw std::runtime_error(archive_error_string(archive));
        return false;
    }
    for (int i = 0; i < fileNames.size(); i++) {
        entry = archive_entry_new();
        archive_entry_set_pathname(entry, fileNames[i].c_str());
        archive_entry_set_size(entry, fileSizes[i]);
        archive_entry_set_filetype(entry, AE_IFREG);
        archive_entry_set_perm(entry, 0644);
        archive_write_header(archive, entry);

        archive_write_data(archive, fileData[i], fileSizes[i]);
        archive_entry_free(entry);
    }
    if (archive_write_close(archive) != ARCHIVE_OK) {
        throw std::runtime_error(archive_error_string(archive));
        return false;
    }
    archive_write_free(archive);

    return true;
}

std::vector<const char*> extractBuffer(std::string archivePath) {
    archive_t archive;
    archive_entry_t entry;
    int response;
    std::vector<const char*> output;
    size_t totalsize = 0;

    archive = archive_read_new();
    archive_read_support_format_all(archive);
    archive_read_support_compression_all(archive);
    if ((response = archive_read_open_filename(archive, archivePath.c_str(), BLOCK_SIZE))) {
        throw std::runtime_error(archive_error_string(archive));
        return std::vector<const char*>();
    }

    while ((response = archive_read_next_header(archive, &entry)) != ARCHIVE_EOF) {
        std::vector<char> current;
        if (response < ARCHIVE_WARN) {
            throw std::runtime_error(archive_error_string(archive));
            return std::vector<const char*>();
        } else if (archive_entry_size(entry) > 0) {
            const void* buffer;
            size_t size;
            int64_t offset;
            while ((response = archive_read_data_block(archive, &buffer, &size, &offset)) != ARCHIVE_EOF) {
                if (response < ARCHIVE_OK) {
                    throw std::runtime_error(archive_error_string(archive));
                    return std::vector<const char*>();
                }
                totalsize += size;
                if (size > 0) {
                    current = cat(current, (char*)buffer, size);
                }
            }
        }
        output.push_back(current.data());
    }
    archive_read_free(archive);
    return output;
}

bool appendBuffer(std::vector<std::string> fileNames, std::vector<const char*> fileData, std::vector<size_t> fileSizes, std::string archivePath) {
    std::vector<const char*> contentData = extractBuffer(archivePath);
    std::vector<metadata_t> contentMeta = view(archivePath);

    for (int i = 0; i < contentMeta.size(); i++) {
        fileData.push_back(contentData[i]);
        fileNames.push_back(contentMeta[i].name);
        fileSizes.push_back((size_t)contentMeta[i].size);
    }
    return writeBuffer(fileNames, fileData, fileSizes, archivePath);
}

#pragma endregion

#pragma region //Worker Classes

class ViewWorker : public Nan::AsyncWorker {
  private:
    std::string path;
    std::vector<metadata_t> files;

  public:
    ViewWorker(Nan::Callback* callback, std::string path)
        : AsyncWorker(callback) {
        this->path = path;
    }

    void Execute() {
        try {
            files = view(path);
        } catch (std::exception& e) {
            this->SetErrorMessage(e.what());
        }
    }

    void HandleOKCallback() {
        Nan::HandleScope scope;
        Local<Array> output = Nan::New<Array>(files.size());
        for (int i = 0; i < files.size(); i++) {
            Local<Object> file = Nan::New<Object>();
            Nan::Set(file, Nan::New<String>("name").ToLocalChecked(), Nan::New<String>(files[i].name).ToLocalChecked());
            Nan::Set(file, Nan::New<String>("size").ToLocalChecked(), Nan::New<Number>(files[i].size));
            Nan::Set(file, Nan::New<String>("directory").ToLocalChecked(), Nan::New<Boolean>(files[i].isDir));
            Nan::Set(output, i, file);
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

class WriteBufferWorker : public Nan::AsyncWorker {
  private:
    std::vector<std::string> fileNames;
    std::vector<const char*> fileData;
    std::vector<size_t> fileSizes;
    std::string archivePath;
    bool outcome;

  public:
    WriteBufferWorker(Callback* callback, std::vector<std::string> fileNames, std::vector<const char*> fileData, std::vector<size_t> fileSizes, std::string archivePath)
        : AsyncWorker(callback) {
        this->fileNames = fileNames;
        this->fileData = fileData;
        this->fileSizes = fileSizes;
        this->archivePath = archivePath;
    }

    void Execute() {
        try {
            outcome = writeBuffer(fileNames, fileData, fileSizes, archivePath);
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

class ReadWorker : public Nan::AsyncWorker {
  private:
    std::string internalPath, archivePath;
    std::vector<char> output;

  public:
    ReadWorker(Callback* callback, std::string internalPath, std::string archivePath) : AsyncWorker(callback) {
        this->internalPath = internalPath;
        this->archivePath = archivePath;
    }

    void Execute() {
        try {
            output = getData(internalPath, archivePath);
        } catch (std::exception& e) {
            this->SetErrorMessage(e.what());
        }
    }

    void HandleOKCallback() {
        Nan::HandleScope scope;
        if (output.size()) {
            Local<Value> argv[] = {
                Nan::Null(),
                Nan::CopyBuffer(output.data(), output.size()).ToLocalChecked()};
            callback->Call(2, argv);
        } else {
            Local<Value> argv[] = {
                Nan::Null(),
                Nan::Undefined()};
            callback->Call(2, argv);
        }
    }

    void HandleErrorCallback() {
        Nan::HandleScope scope;
        Local<Value> argv[] = {
            Nan::New<String>(this->ErrorMessage()).ToLocalChecked(),
            Nan::Null()};
        callback->Call(2, argv);
    }
};

class AppendWorker : public Nan::AsyncWorker {
  private:
    std::vector<std::string> newFiles;
    std::string archivePath;
    bool outcome;

  public:
    AppendWorker(Callback* callback, std::vector<std::string> newFiles, std::string archivePath) : AsyncWorker(callback) {
        this->newFiles = newFiles;
        this->archivePath = archivePath;
    }

    void Execute() {
        try {
            outcome = appendLocal(newFiles, archivePath);
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

class AppendBufferWorker : public Nan::AsyncWorker {
  private:
    std::vector<std::string> fileNames;
    std::vector<const char*> fileData;
    std::vector<size_t> fileSizes;
    std::string archivePath;
    bool outcome;

  public:
    AppendBufferWorker(Callback* callback, std::vector<std::string> fileNames, std::vector<const char*> fileData, std::vector<size_t> fileSizes, std::string archivePath)
        : AsyncWorker(callback) {
        this->fileNames = fileNames;
        this->fileData = fileData;
        this->fileSizes = fileSizes;
        this->archivePath = archivePath;
    }

    void Execute() {
        try {
            outcome = appendBuffer(fileNames, fileData, fileSizes, archivePath);
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

#pragma endregion

#pragma region //Wrappers

NAN_METHOD(Content) {
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

NAN_METHOD(Create) {
    if (info.Length() == 3) {
        if (!info[0]->IsArray()) {
            Nan::ThrowError("Usage: Create(array: fileNames, ?array: buffers, string: archivePath, function: callback)");
            return;
        }
        if (!info[1]->IsString()) {
            Nan::ThrowError("Usage: Create(array: fileNames, ?array: buffers, string: archivePath, function: callback)");
            return;
        }
        if (!info[2]->IsFunction()) {
            Nan::ThrowError("Usage: Create(array: fileNames, ?array: buffers, string: archivePath, function: callback)");
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
    } else if (info.Length() == 4) {
        if (!info[0]->IsArray()) {
            Nan::ThrowError("Usage: Create(array: fileNames, ?array: buffers, string: archivePath, function: callback)");
            return;
        }
        if (!info[1]->IsArray()) {
            Nan::ThrowError("Usage: Create(array: fileNames, ?array: buffers, string: archivePath, function: callback)");
            return;
        }
        if (!info[2]->IsString()) {
            Nan::ThrowError("Usage: Create(array: fileNames, ?array: buffers, string: archivePath, function: callback)");
            return;
        }
        if (!info[3]->IsFunction()) {
            Nan::ThrowError("Usage: Create(array: fileNames, ?array: buffers, string: archivePath, function: callback)");
            return;
        }
        std::vector<std::string> files;
        std::vector<const char*> buffers;
        std::vector<size_t> sizes;
        for (int i = 0; i < Local<Array>::Cast(info[0])->Length(); i++) {
            files.push_back(std::string(*String::Utf8Value(Local<Array>::Cast(info[0])->Get(i))));
            buffers.push_back((const char*)node::Buffer::Data(Local<Array>::Cast(info[1])->Get(i)->ToObject()));
            sizes.push_back(node::Buffer::Length(Local<Array>::Cast(info[1])->Get(i)->ToObject()));
        }

        Callback* callback = new Callback(Nan::To<Function>(info[3]).ToLocalChecked());
        Nan::AsyncQueueWorker(new WriteBufferWorker(callback, files, buffers, sizes, std::string(*String::Utf8Value(info[2]->ToString()))));
    } else {
        Nan::ThrowError("Usage: Create(array: fileNames, ?array: buffers, string: archivePath, function: callback)");
    }
}

NAN_METHOD(Append) {
    if (info.Length() == 3) {
        if (!info[0]->IsArray()) {
            Nan::ThrowError("Usage: Append(array: fileNames, ?array: buffers, string: archivePath, function: callback)");
            return;
        }
        if (!info[1]->IsString()) {
            Nan::ThrowError("Usage: Append(array: fileNames, ?array: buffers, string: archivePath, function: callback)");
            return;
        }
        if (!info[2]->IsFunction()) {
            Nan::ThrowError("Usage: Append(array: fileNames, ?array: buffers, string: archivePath, function: callback)");
            return;
        }
        Callback* callback = new Callback(Nan::To<Function>(info[2]).ToLocalChecked());
        std::vector<std::string> newFiles;
        for (int i = 0; i < Local<Array>::Cast(info[0])->Length(); i++) {
            newFiles.push_back(std::string(*String::Utf8Value(Local<Array>::Cast(info[0])->Get(i))));
        }
        Nan::AsyncQueueWorker(new AppendWorker(callback, newFiles, std::string(*String::Utf8Value(info[1]->ToString()))));
    } else if (info.Length() == 4) {
        if (!info[0]->IsArray()) {
            Nan::ThrowError("Usage: Append(array: fileNames, ?array: buffers, string: archivePath, function: callback)");
            return;
        }
        if (!info[1]->IsArray()) {
            Nan::ThrowError("Usage: Append(array: fileNames, ?array: buffers, string: archivePath, function: callback)");
            return;
        }
        if (!info[2]->IsString()) {
            Nan::ThrowError("Usage: Append(array: fileNames, ?array: buffers, string: archivePath, function: callback)");
            return;
        }
        if (!info[3]->IsFunction()) {
            Nan::ThrowError("Usage: Append(array: fileNames, ?array: buffers, string: archivePath, function: callback)");
            return;
        }
        std::vector<std::string> files;
        std::vector<const char*> buffers;
        std::vector<size_t> sizes;
        for (int i = 0; i < Local<Array>::Cast(info[0])->Length(); i++) {
            files.push_back(std::string(*String::Utf8Value(Local<Array>::Cast(info[0])->Get(i))));
            buffers.push_back((const char*)node::Buffer::Data(Local<Array>::Cast(info[1])->Get(i)->ToObject()));
            sizes.push_back(node::Buffer::Length(Local<Array>::Cast(info[1])->Get(i)->ToObject()));
        }
        Callback* callback = new Callback(Nan::To<Function>(info[3]).ToLocalChecked());
        Nan::AsyncQueueWorker(new AppendBufferWorker(callback, files, buffers, sizes, std::string(*String::Utf8Value(info[2]->ToString()))));
    } else {
        Nan::ThrowError("Usage: Append(array: fileNames, ?array: buffers, string: archivePath, function: callback)");
    }
}

NAN_METHOD(Extract) {
    if (info.Length() == 2) {
        if (!info[0]->IsString()) {
            Nan::ThrowError("archivePath Must be string");
            return;
        }
        if (!info[1]->IsFunction()) {
            Nan::ThrowError("callback Must be function");
            return;
        }

        String::Utf8Value ap(info[0]->ToString());
        Callback* callback = new Callback(Nan::To<Function>(info[1]).ToLocalChecked());
        Nan::AsyncQueueWorker(new ExtractWorker(callback, std::string(*ap), std::string("./")));
    } else if (info.Length() == 3) {
        if (!info[0]->IsString()) {
            Nan::ThrowError("archivePath Must be string");
            return;
        }
        if (!info[1]->IsString()) {
            Nan::ThrowError("outputPath Must be string");
            return;
        }
        if (!info[2]->IsFunction()) {
            Nan::ThrowError("callback Must be function");
            return;
        }
        String::Utf8Value ap(info[0]->ToString());
        String::Utf8Value op(info[1]->ToString());
        Callback* callback = new Callback(Nan::To<Function>(info[2]).ToLocalChecked());
        Nan::AsyncQueueWorker(new ExtractWorker(callback, std::string(*ap), std::string(*op)));
    } else {
        Nan::ThrowError("Usage: Extract(archivePath, optional: outputPath, callback)");
    }
}

NAN_METHOD(Read) {
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
        Nan::AsyncQueueWorker(new ReadWorker(callback, std::string(*String::Utf8Value(info[0]->ToString())), std::string(*String::Utf8Value(info[1]->ToString()))));
    } else {
        Nan::ThrowError("Usage: ReadBuffer(internalPath, archivePath, callback)");
    }
}

#pragma endregion

#pragma region //Node

NAN_MODULE_INIT(init) {
    Nan::Set(target, New<String>("Content").ToLocalChecked(),
             GetFunction(New<FunctionTemplate>(Content)).ToLocalChecked());

    Nan::Set(target, New<String>("Create").ToLocalChecked(),
             GetFunction(New<FunctionTemplate>(Create)).ToLocalChecked());

    Nan::Set(target, New<String>("Append").ToLocalChecked(),
             GetFunction(New<FunctionTemplate>(Append)).ToLocalChecked());

    Nan::Set(target, New<String>("Extract").ToLocalChecked(),
             GetFunction(New<FunctionTemplate>(Extract)).ToLocalChecked());

    Nan::Set(target, New<String>("Read").ToLocalChecked(),
             GetFunction(New<FunctionTemplate>(Read)).ToLocalChecked());
}

NODE_MODULE(manager, init);

#pragma endregion

} // namespace archive_manager