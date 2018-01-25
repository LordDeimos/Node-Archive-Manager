#include <node.h>
#include "./lib/archive.h"
#include "./lib/archive_entry.h"

namespace demo {

using v8::Exception;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::Number;
using v8::String;
using v8::Value;

const char* ToCString(Local<String> str) {
  String::Utf8Value value(str);
  return *value ? *value : "<string conversion failed>";
}

void view(Local<String> path, Isolate* isolate){
  struct archive *a;
  struct archive_entry *entry;
  int r;

  a = archive_read_new();
  archive_read_support_filter_all(a);
  archive_read_support_format_all(a);
  r = archive_read_open_filename(a, ToCString(path), 10240); // Note 1
  if (r != ARCHIVE_OK)
    return;
  while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
    printf("%s\\n",archive_entry_pathname(entry));
    archive_read_data_skip(a);  // Note 2
  }
  r = archive_read_free(a);  // Note 3
  if (r != ARCHIVE_OK)
    return;
}

void Method(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  if(args.Length()!=1){
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate,"This Only Takes One Arguments")));
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