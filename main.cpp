#include <node.h>
#include <archive.h>
#include <archive_entry.h>

namespace demo {

using v8::Exception;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::Number;
using v8::String;
using v8::Value;

typedef struct archive* archive_t;
typedef struct archive_entry* archive_entry_t;

const char* ToCString(Local<String> str) {
  String::Utf8Value value(str);
  return *value ? *value : "<string conversion failed>";
}

void view(Local<String> path, Isolate* isolate){
  archive_t archive;
  archive_entry_t entry;
  int r;

  archive = archive_read_new();
  archive_read_support_filter_all(archive);
  archive_read_support_format_all(archive);

  String::Utf8Value file(path);

  printf("%s\n",*file);
  r = archive_read_open_filename(archive, *file, 10240);
  if (r != ARCHIVE_OK)
    printf("Error 1\n");

  while (archive_read_next_header(archive, &entry) == ARCHIVE_OK) {
    printf("%s\n",archive_entry_pathname(entry));
    archive_read_data_skip(archive);  // Note 2
  }

  r = archive_read_free(archive);

  if (r != ARCHIVE_OK)
    printf("Error 2\n");

}

void Method(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  if(args.Length()!=1){
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate,"This Takes One Argument")));
      return;
  }
  view(args[0]->ToString(),isolate);
  args.GetReturnValue().Set(String::NewFromUtf8(isolate,"Printed to stdout"));
}

void init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "view", Method);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, init)

}