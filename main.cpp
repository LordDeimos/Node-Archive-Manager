#include <string.h>
#include <nan.h>

#include <archive.h>
#include <archive_entry.h>

namespace archive_manager {

using namespace v8;
using namespace Nan;

typedef struct archive* archive_t;
typedef struct archive_entry* archive_entry_t;

#pragma region Helpers

Local<Array> view(Local<String> path, Isolate* isolate){
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
  while (archive_read_next_header(archive, &entry) == ARCHIVE_OK) {
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

Local<Object> info(Local<String> fileName, Local<String> archivePath, Isolate* isolate){
  archive_t archive;
  archive_entry_t entry;
  int r;
  archive = archive_read_new();
  archive_read_support_filter_all(archive);
  archive_read_support_format_all(archive);

  String::Utf8Value file(archivePath);
  String::Utf8Value internalFile(fileName);

  Local<Object> object = New<Object>();
  r = archive_read_open_filename(archive, *file, 10240);
  if (r != ARCHIVE_OK){
    Nan::ThrowError("Error Opening Archive");
    return New<Object>();
  }
  printf("%s\n",*internalFile);
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

Local<Boolean> writeLocal(Local<String> fileName, Local<String> archivePath, Isolate* isolate){
  //This might take some doing
  return Nan::False();
}

Local<Boolean> writeMemory(Local<String> file, Local<String> archivePath, Isolate* isolate){
  //This might take some more doing
  return Nan::False();
}

/**
 * ToDo
 * - Extract to/from disk
 * - Create New archive
 * - Remove file/folder from archive
*/

#pragma endregion

#pragma region Wrappers

void ListContent(const Nan::FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  if(args.Length()!=1){
    Nan::ThrowError("This Takes One Argument");
    return;
  }
  args.GetReturnValue().Set(view(args[0]->ToString(),isolate));
}

void GetInfo(const Nan::FunctionCallbackInfo<Value>& args){
  Isolate* isolate = args.GetIsolate();
  if(args.Length()!=2){
    Nan::ThrowError("Requires two arguments");
    return;
  }
  args.GetReturnValue().Set(info(args[0]->ToString(),args[1]->ToString(),isolate));
}

#pragma endregion

#pragma region Node

NAN_MODULE_INIT(init) {
  /*Isolate* isolate = exports->GetIsolate();

  exports->Set(String::NewFromUtf8(isolate,"ListContent"),FunctionTemplate::New(isolate,ListContent)->GetFunction());
  exports->Set(String::NewFromUtf8(isolate,"GetInfo"),FunctionTemplate::New(isolate,GetInfo)->GetFunction());*/

  Nan::Set(target, New<String>("ListContent").ToLocalChecked(),
    GetFunction(New<FunctionTemplate>(ListContent)).ToLocalChecked());

  Nan::Set(target, New<String>("GetInfo").ToLocalChecked(),
    GetFunction(New<FunctionTemplate>(GetInfo)).ToLocalChecked());

}

NODE_MODULE(NODE_GYP_MODULE_NAME, init);

#pragma endregion

}