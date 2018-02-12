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

#pragma region Helpers

Local<Array> view(Local<String> path){
  archive_t archive;
  archive_entry_t entry;
  int r;

  archive = archive_read_new();
  archive_read_support_filter_all(archive);
  archive_read_support_format_all(archive);

  String::Utf8Value file(path);
  Local<Array> array = New<Array>();

  r = archive_read_open_filename(archive, *file, 10240);
  if (r != ARCHIVE_OK){
    Nan::ThrowError("Error Opening Archive");
    return New<Array>();
  }
  int i=0;
  while (archive_read_next_header(archive, &entry) != ARCHIVE_EOF) {
    Nan::Set(array,i,New<String>(archive_entry_pathname(entry)).ToLocalChecked());
    archive_read_data_skip(archive);
    i++;
  }

  r = archive_read_free(archive);

  if (r != ARCHIVE_OK){    
    Nan::ThrowError("Error Closing Archive");
    return New<Array>();
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
    Nan::ThrowError("Error Opening Archive");
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
    Nan::ThrowError("Error Closing Archive");
    return New<Object>();
  }
  return object;
}

Local<Boolean> writeLocal(Local<Array> files, Local<String> archivePath){
  //This might take some doing
  archive_t archive;
  archive_entry_t entry;
  struct stat st;
  char buff[8192];
  int len;
  int fd;
  
  String::Utf8Value file(archivePath);
  
  archive = archive_write_new();
  archive_write_set_format_filter_by_ext(archive,*file);
  if(archive_write_open_filename(archive, *file)!=ARCHIVE_OK){
    Nan::ThrowError("Error Opening Archive");
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
    fd = open(*filename, O_RDONLY);
    len = read(fd, buff, sizeof(buff));
    while ( len > 0 ) {
      archive_write_data(archive, buff, len);
      len = read(fd, buff, sizeof(buff));
    }
    close(fd);
    archive_entry_free(entry);
  }
  if(archive_write_close(archive)!=ARCHIVE_OK){
    Nan::ThrowError("Error Closing Archive");
    return Nan::False();
  }
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
    Nan::ThrowError("Error Opening Archive");
    return Nan::False();
  }

  while((response=archive_read_next_header(archive,&entry))!=ARCHIVE_EOF){
    if(response<ARCHIVE_WARN){
      Nan::ThrowError("Corrupt Archive");
      return Nan::False();
    }
    std::string internal(archive_entry_pathname(entry));
    std::string path(*outPath);
    archive_entry_set_pathname(entry,(path+internal).c_str());
    if((response=archive_write_header(archivew,entry))!=ARCHIVE_OK){
      Nan::ThrowError("Error Writing Header");
      return Nan::False();
    }
    else if(archive_entry_size(entry) > 0){
      const void* buffer;
      size_t size;
      la_int64_t offset;
      while((response = archive_read_data_block(archive,&buffer,&size,&offset))!=ARCHIVE_EOF){
        if(response<ARCHIVE_OK){
          Nan::ThrowError("Error Reading Data");
          return Nan::False();
        }
        if((response = archive_write_data_block(archivew,buffer,size,offset))<ARCHIVE_OK){
          Nan::ThrowError("Error Writing Data");
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

  Local<Array> oldFiles = view(archivePath);

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
  for(char entry : left){
    output.push_back(entry);
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
    Nan::ThrowError("Error Opening Archive");
    return Nan::New<Object>();
  }

  while((response=archive_read_next_header(archive,&entry))!=ARCHIVE_EOF){
    if(response<ARCHIVE_WARN){
      Nan::ThrowError("Corrupt Archive");
      return Nan::New<Object>();
    }
    else if(archive_entry_size(entry) > 0){
      if(!strcmp(archive_entry_pathname(entry),*internalFile)){
        const void* buffer;
        size_t size;
        la_int64_t offset;
        while((response = archive_read_data_block(archive,&buffer,&size,&offset))!=ARCHIVE_EOF){
          if(response<ARCHIVE_OK){
            Nan::ThrowError("Error Reading Data");
            return Nan::New<Object>();
          }
          totalsize+=size;
          if(size>0){
            output = cat(output, (char*)buffer, size);
          }
          break;
        }
      }
    }
  }
  archive_read_free(archive);
  return Nan::CopyBuffer(output.data(), static_cast<uint32_t>(output.size())).ToLocalChecked();
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

#pragma region Wrappers

void ListContent(const Nan::FunctionCallbackInfo<Value>& args) {
  if(args.Length()!=1){
    Nan::ThrowError("This Takes One Argument");
    return;
  }
  args.GetReturnValue().Set(view(args[0]->ToString()));
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

#pragma region Node

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

NODE_MODULE(archive_manager, init);

#pragma endregion

}