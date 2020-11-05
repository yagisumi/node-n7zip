#include "create_reader2.h"
#include "create_reader_args.h"
#include "create_reader_worker.h"

namespace n7zip {

static Napi::Object
createReader(const Napi::CallbackInfo& info)
{
  TRACE("createReader2");
  auto env = info.Env();

  if ((info.Length() == 0) || (info[0].IsArray() || !info[0].IsObject())) {
    return TYPE_ERR(env, "The first argument must be Object");
  }

  if ((info.Length() < 2) || !info[1].IsFunction()) {
    return TYPE_ERR(env, "The second argument must be callback function");
  }

  auto arg = info[0].ToObject();
  auto result = buildCreateReaderArg(arg);
  if (result.err()) {
    return result.err()->ERR(env);
  } else if (!result.ok()) {
    return ERR(env, "Unexpected error");
  }

  new CreateReaderWorker(result.move_ok(), env, info[1].As<Napi::Function>());

  return OK(env);
}

Napi::Object
InitReader2(Napi::Env env, Napi::Object exports)
{
  // ReaderWrap::Init(env, exports);
  exports.Set("createReader2", Napi::Function::New(env, createReader));

  return exports;
}

} // namespace n7zip
