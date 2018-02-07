#include <string.h>
#include <node.h>
#include <archive.h>
#include <archive_entry.h>

namespace archive_manager {

using namespace v8;

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
  Local<Array> array = Array::New(isolate);

  r = archive_read_open_filename(archive, *file, 10240);
  if (r != ARCHIVE_OK){
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate,"Error Opening Archive")));
      return Array::New(isolate);
  }
  int i=0;
  while (archive_read_next_header(archive, &entry) == ARCHIVE_OK) {
    array->Set(i,String::NewFromUtf8(isolate,archive_entry_pathname(entry)));
    archive_read_data_skip(archive);
    i++;
  }

  r = archive_read_free(archive);

  if (r != ARCHIVE_OK){    
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate,"Error Closing Archive")));
      return Array::New(isolate);
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

  Local<Object> object = Object::New(isolate);
  r = archive_read_open_filename(archive, *file, 10240);
  if (r != ARCHIVE_OK){
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate,"Error Opening Archive")));
      return Object::New(isolate);
  }
  printf("%s\n",*internalFile);
  while (archive_read_next_header(archive, &entry) == ARCHIVE_OK) {
    if(!strcmp(archive_entry_pathname(entry),*internalFile)){
      object->Set(String::NewFromUtf8(isolate,"name"),String::NewFromUtf8(isolate,archive_entry_pathname(entry)));
      object->Set(String::NewFromUtf8(isolate,"size"),Number::New(isolate,archive_entry_size(entry)));
      object->Set(String::NewFromUtf8(isolate,"directory"),Boolean::New(isolate,archive_entry_filetype(entry)==AE_IFDIR));
      //more?
      break;
    }
  }

  r = archive_read_free(archive);

  if (r != ARCHIVE_OK){    
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate,"Error Closing Archive")));
      return Object::New(isolate);
  }
  return object;
}

Local<Boolean> writeLocal(Local<String> fileName, Local<String> archivePath, Isolate* isolate){
  //This might take some doing
  return Boolean::new(isolate,false);
}

Local<Boolean> writeMemory(Local<Buffer> file, Local<String> archivePath, Isolate* isolate){
  //This might take some more doing
  return Boolean::new(isolate,false);
}

/**
 * ToDo
 * - Extract to/from disk
 * - Create New archive
 * - Remove file/folder from archive
*/

#prag1ma endregion

#pragma region Wrappers

void ListContent(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  if(args.Length()!=1){
    isolate->ThrowException(Exception::SyntaxError(
      String::NewFromUtf8(isolate,"This Takes One Argument")));
      return;
  }
  args.GetReturnValue().Set(view(args[0]->ToString(),isolate));
}

void GetInfo(const FunctionCallbackInfo<Value>& args){
  Isolate* isolate = args.GetIsolate();
  if(args.Length()!=2){
    isolate->ThrowException(Exception::SyntaxError(
      String::NewFromUtf8(isolate,"Requires two arguments")
    ));
    return;
  }
  args.GetReturnValue().Set(info(args[0]->ToString(),args[1]->ToString(),isolate));
}

#pragma endregion

#pragma region Node

void init(Local<Object> exports) {
  Isolate* isolate = exports->GetIsolate();

  exports->Set(String::NewFromUtf8(isolate,"ListContent"),FunctionTemplate::New(isolate,ListContent)->GetFunction());
  exports->Set(String::NewFromUtf8(isolate,"GetInfo"),FunctionTemplate::New(isolate,GetInfo)->GetFunction());

}

NODE_MODULE(NODE_GYP_MODULE_NAME, init);

#pragma endregion

}