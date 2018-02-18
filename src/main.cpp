#include <string.h>
#include <iostream>
#include <sstream>
#include <nan.h>

#include <archive.h>
#include <archive_entry.h>

#define BLOCK_SIZE 10240

namespace archive_manager {

using namespace v8;
using namespace Nan;

typedef struct archive* archive_t;
typedef struct archive_entry* archive_entry_t;

std::vector<std::string> split(char* string, char delim);

#pragma region //Helpers

std::vector<std::string> view(const char* file){
  archive_t archive;
  archive_entry_t entry;
  int r;

  archive = archive_read_new();
  archive_read_support_filter_all(archive);
  archive_read_support_format_all(archive);

  std::vector<std::string> array;

  r = archive_read_open_filename(archive, file, 10240);
  if (r != ARCHIVE_OK){
    throw std::exception(archive_error_string(archive));
    return std::vector<std::string>();
  }
  int i=0;
  while (archive_read_next_header(archive, &entry) != ARCHIVE_EOF) {
    array.push_back(std::string(archive_entry_pathname(entry)));
    archive_read_data_skip(archive);
    i++;
  }
  r = archive_read_close(archive);
  r = archive_read_free(archive);

  if (r != ARCHIVE_OK){
    throw std::exception(archive_error_string(archive));
    return std::vector<std::string>();
  }
  return array;
}

Local<Object> info(Local<String> fileName, Local<String> archivePath){
  archive_t archive;
  archive_entry_t entry;
  int r;
  archive = archive_read_new();
  archive_read_support_filter_all(archive);
  archive_read_support_format_all(archive);

  String::Utf8Value file(archivePath);
  String::Utf8Value internalFile(fileName);

  Local<Object> object = New<Object>();
  r = archive_read_open_filename(archive, *file, BLOCK_SIZE);
  if (r != ARCHIVE_OK){
    Nan::ThrowError(archive_error_string(archive));
    return New<Object>();
  }
  while (archive_read_next_header(archive, &entry) == ARCHIVE_OK) {
    if(!strcmp(archive_entry_pathname(entry),*internalFile)){
      Nan::Set(object,New<String>("name").ToLocalChecked(),New<String>(archive_entry_pathname(entry)).ToLocalChecked());
      Nan::Set(object,New<String>("size").ToLocalChecked(),New<Number>(archive_entry_size(entry)));
      Nan::Set(object,New<String>("directory").ToLocalChecked(),New<Boolean>(archive_entry_filetype(entry)==AE_IFDIR));
      //more?
      break;
    }
  }

  r = archive_read_free(archive);

  if (r != ARCHIVE_OK){    
    Nan::ThrowError(archive_error_string(archive));
    return New<Object>();
  }
  return object;
}

void set_filter(archive_t archive, char* file){
  std::vector<std::string> tokens = split(file,'.');
  if(tokens.size()>=2){
    if(!tokens.back().compare("7zip")||!tokens.back().compare("cb7")){
      archive_write_set_format_7zip(archive);
      archive_write_add_filter_none(archive);
    }
    else if(!tokens.back().compare("zip")||!tokens.back().compare("cbz")){
      archive_write_set_format_zip(archive);
      archive_write_add_filter_none(archive);
    }else if(!tokens.back().compare("jar")){
      archive_write_set_format_zip(archive);
      archive_write_add_filter_none(archive);
    }else if(!tokens.back().compare("cpio")){
      archive_write_set_format_cpio(archive);
      archive_write_add_filter_none(archive);
    }else if(!tokens.back().compare("iso")){
      archive_write_set_format_iso9660(archive);
      archive_write_add_filter_none(archive);      
    }else if(!tokens.back().compare("tar")){
      archive_write_set_format_pax_restricted(archive);
      archive_write_add_filter_none(archive);      
    }else if(!tokens.back().compare("tgz")){
      archive_write_set_format_pax_restricted(archive);
      archive_write_add_filter_gzip(archive);
    }else if(!tokens.back().compare("gz")){
      if(tokens[tokens.size()-2]=="tar"){                  
        archive_write_set_format_pax_restricted(archive);
        archive_write_add_filter_gzip(archive);
      }
    }else if(!tokens.back().compare("bz2")){
      if(tokens[tokens.size()-2]=="tar"){                  
        archive_write_set_format_pax_restricted(archive);
        archive_write_add_filter_bzip2(archive);
      }
    }else if(!tokens.back().compare("xz")){
      if(tokens[tokens.size()-2]=="tar"){                  
        archive_write_set_format_pax_restricted(archive);
        archive_write_add_filter_xz(archive);
      }
    }else if(!tokens.back().compare("a")||!tokens.back().compare("ar")){
      #if defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__NetBSD__) || defined(__OpenBSD__)
        archive_write_set_format_ar_bsd(archive);
        archive_write_add_filter_none(archive);
      #else
        archive_write_set_format_ar_svr4(archive);
        archive_write_add_filter_none(archive);
      #endif
    }else{      
      archive_write_set_format_zip(archive);
      archive_write_add_filter_none(archive);
    }
  }
}

Local<Boolean> writeLocal(Local<Array> files, Local<String> archivePath){
  //This might take some doing
  archive_t archive;
  archive_entry_t entry;
  struct stat st;
  char buff[BLOCK_SIZE];
  int len;
  FILE* fd = NULL;
  
  String::Utf8Value file(archivePath);
  
  archive = archive_write_new();

  //archive_write_set_format_filter_by_ext(archive,*file); only for libarchive >=3.2
  set_filter(archive,*file);

  if(archive_write_open_filename(archive, *file)!=ARCHIVE_OK){
    Nan::ThrowError(archive_error_string(archive));
    return Nan::False();
  }
  for(int i=0;i<files->Length();i++){    
    String::Utf8Value filename(files->Get(i));
    std::vector<std::string> path = split(*filename,'/');  

    const char * internalName = path.back().c_str();
    stat(*filename, &st);
    entry = archive_entry_new();

    archive_entry_set_pathname(entry, internalName);
    archive_entry_set_size(entry, st.st_size);
    archive_entry_set_filetype(entry, AE_IFREG);
    archive_entry_set_perm(entry, 0644);
    archive_write_header(archive, entry);
    fd = fopen(*filename, "r");
    if(fd){
      len = fread(buff, sizeof(buff), sizeof(char),fd);
      while ( len > 0 ) {
        archive_write_data(archive, buff, len);
        len = fread(buff, sizeof(buff), sizeof(char),fd);
      }
      fclose(fd);
    }
    archive_entry_free(entry);
  }
  if(archive_write_close(archive)!=ARCHIVE_OK){
    Nan::ThrowError(archive_error_string(archive));
    return Nan::False();
  }
  archive_write_close(archive);
  archive_write_free(archive);

  return Nan::True();
}

std::vector<std::string> split(char* string, char delim){
  std::vector<std::string> tokens;
  std::string input(string);
  std::istringstream ss(input);
  std::string token;

  while(std::getline(ss, token, delim)) {
    tokens.push_back(token);
  }
  return tokens;
}

Local<Boolean> extract(Local<String> archivePath, Local<String> outputPath){

  String::Utf8Value filename(archivePath);
  String::Utf8Value outPath(outputPath);

  archive_t archive, archivew;
  archive_entry_t entry;
  int response, flags = ARCHIVE_EXTRACT_TIME|ARCHIVE_EXTRACT_PERM|ARCHIVE_EXTRACT_ACL|ARCHIVE_EXTRACT_FFLAGS;

  archive = archive_read_new();
  archive_read_support_format_all(archive);
  archive_read_support_compression_all(archive);
  archivew = archive_write_disk_new();
  archive_write_disk_set_options(archivew, flags);
  archive_write_disk_set_standard_lookup(archivew);

  if((response=archive_read_open_filename(archive,*filename,BLOCK_SIZE))){
    Nan::ThrowError(archive_error_string(archive));
    return Nan::False();
  }

  while((response=archive_read_next_header(archive,&entry))!=ARCHIVE_EOF){
    if(response<ARCHIVE_WARN){
      Nan::ThrowError(archive_error_string(archive));
      return Nan::False();
    }
    std::string internal(archive_entry_pathname(entry));
    std::string path(*outPath);
    archive_entry_set_pathname(entry,(path+internal).c_str());
    if((response=archive_write_header(archivew,entry))!=ARCHIVE_OK){
      Nan::ThrowError(archive_error_string(archive));
      return Nan::False();
    }
    else if(archive_entry_size(entry) > 0){
      const void* buffer;
      size_t size;
      int64_t offset;
      while((response = archive_read_data_block(archive,&buffer,&size,&offset))!=ARCHIVE_EOF){
        if(response<ARCHIVE_OK){
          Nan::ThrowError(archive_error_string(archive));
          return Nan::False();
        }
        if((response = archive_write_data_block(archivew,buffer,size,offset))<ARCHIVE_OK){
          Nan::ThrowError(archive_error_string(archive));
          return Nan::False();
        }
      }
    }
  }
  archive_read_free(archive);
  archive_write_free(archivew);

  return Nan::True();
}

Local<Boolean> appendLocal(Local<Array> newFiles, Local<String> archivePath){
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
  for(int i=0;i<content.size();i++){
    Nan::Set(oldFiles,i,Nan::New<String>(content[i].c_str()).ToLocalChecked());
  }

  for(int i=0;i<oldFiles->Length();i++){
    String::Utf8Value temp(oldFiles->Get(i)->ToString());
    std::string path(*temp);
    Nan::Set(newFiles,newFiles->Length(),Nan::New<String>(tempDir+path).ToLocalChecked());
  }

  if(extract(archivePath,Nan::New<String>(tempDir).ToLocalChecked())->BooleanValue()){
    return writeLocal(newFiles, archivePath);
  }

  return Nan::False();
}

std::vector<char> cat(std::vector<char> left, char* right, int rightSize){
  std::vector<char> output;
  for(int i=0;i<left.size();i++){
    output.push_back(left[i]);
  }
  for(int i=0;i<rightSize;i++){    
    output.push_back(right[i]);
  }
  return output;
}

Local<Object> getData(Local<String> internalPath, Local<String> archivePath){
  String::Utf8Value filename(archivePath);
  String::Utf8Value internalFile(internalPath);

  archive_t archive;
  archive_entry_t entry;
  int response;
  std::vector<char> output;
  size_t totalsize=0;

  archive = archive_read_new();
  archive_read_support_format_all(archive);
  archive_read_support_compression_all(archive);
  if((response=archive_read_open_filename(archive,*filename,BLOCK_SIZE))){
    Nan::ThrowError(archive_error_string(archive));
    return Nan::New<Object>();
  }

  while((response=archive_read_next_header(archive,&entry))!=ARCHIVE_EOF){
    if(response<ARCHIVE_WARN){
      Nan::ThrowError(archive_error_string(archive));
      return Nan::New<Object>();
    }
    else if(archive_entry_size(entry) > 0){
      if(!strcmp(archive_entry_pathname(entry),*internalFile)){
        const void* buffer;
        size_t size;
        int64_t offset;
        while((response = archive_read_data_block(archive,&buffer,&size,&offset))!=ARCHIVE_EOF){
          if(response<ARCHIVE_OK){
            Nan::ThrowError(archive_error_string(archive));
            return Nan::New<Object>();
          }
          totalsize+=size;
          if(size>0){
            output = cat(output, (char*)buffer, size);
          }
        }
        break;
      }
    }
  }
  archive_read_free(archive);
  if(output.size()>0){
    return Nan::CopyBuffer(output.data(), static_cast<uint32_t>(output.size())).ToLocalChecked();
  }
  return Nan::New<Object>();
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

class ViewWorker: public Nan::AsyncWorker{
  private:
    std::string path;
    std::vector<std::string> files;

  public: 
    ViewWorker(Nan::Callback* callback, std::string path)
      : AsyncWorker(callback){
        this->path = path;
        this->files = std::vector<std::string>();
      }
    
    void Execute(){
      try{
        files = view(path.c_str());
      }
      catch(std::exception& e){
        this->SetErrorMessage(e.what());
      }
    }

    void HandleOKCallback(){
      Nan::HandleScope scope;      
      Local<Array> output = Nan::New<Array>(files.size());
      for(int i=0;i<files.size();i++){
        Nan::Set(output,i,Nan::New<String>(files[i].c_str()).ToLocalChecked());
      }
      Local<Value> argv[] = {Nan::Null(), output};
      callback->Call(2,argv);
    }
    void HandleErrorCallback(){      
      Nan::HandleScope scope;
      Local<Value> argv[] = {Nan::New<String>(this->ErrorMessage()).ToLocalChecked(),Nan::Null()};
      callback->Call(2,argv);
    }
};

#pragma endregion

#pragma region //Wrappers

NAN_METHOD(ListContent) {
  if(info.Length()!=2){
    Nan::ThrowError("This Takes Two Arguments");
    return;
  }
  if(!info[0]->IsString()){
    Nan::ThrowError("Expected Path as First Arg");
    return;
  }
  if(!info[1]->IsFunction()){
    Nan::ThrowError("Expected Callback as Second Arg");
    return;
  }
  String::Utf8Value utf8path(info[0]->ToString());
  std::string path(*utf8path);
  Callback* callback = new Callback(Nan::To<Function>(info[1]).ToLocalChecked());
  Nan::AsyncQueueWorker(new ViewWorker(callback, path));
}

void GetInfo(const Nan::FunctionCallbackInfo<Value>& args){
  if(args.Length()!=2){
    Nan::ThrowError("Requires two arguments");
    return;
  }
  args.GetReturnValue().Set(info(args[0]->ToString(),args[1]->ToString()));
}

void WriteFromDisk(const Nan::FunctionCallbackInfo<Value>& args){
  if(args.Length()==2){
    if(!args[0]->IsArray()){
      Nan::ThrowError("newFiles Must be Array");
      return;
    }
    args.GetReturnValue().Set(writeLocal(Local<Array>::Cast(args[0]),args[1]->ToString()));
  }
  else{
    Nan::ThrowError("Usage: WriteFromDisk([newFiles], archivePath)");
  }
}

void Append(const Nan::FunctionCallbackInfo<Value>& args){
  if(args.Length()==2){
    if(!args[0]->IsArray()){
      Nan::ThrowError("newFiles Must be Array");
      return;
    }
    args.GetReturnValue().Set(appendLocal(Local<Array>::Cast(args[0]),args[1]->ToString()));
  }
  else{
    Nan::ThrowError("Usage: Append([newFiles], archivePath)");
  }
}

void Extract(const Nan::FunctionCallbackInfo<Value>& args){
  if(args.Length()==1){
    args.GetReturnValue().Set(extract(args[0]->ToString(),Nan::New<String>("./").ToLocalChecked()));
  }
  else if(args.Length()==2){    
    args.GetReturnValue().Set(extract(args[0]->ToString(),args[1]->ToString()));
  }
  else{
    Nan::ThrowError("Usage: Extract(archivePath, outputPath)");
  }
}

void ReadBuffer(const Nan::FunctionCallbackInfo<Value>& args){
  if(args.Length()==2){
    args.GetReturnValue().Set(getData(args[0]->ToString(),args[1]->ToString()));
  }
  else{
    Nan::ThrowError("Usage: ReadBuffer(internalPath, archivePath)");
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

}