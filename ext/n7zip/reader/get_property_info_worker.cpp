#include "get_property_info_worker.h"

namespace n7zip {

GetPropertyInfoWorker::GetPropertyInfoWorker(Reader* reader, Napi::Env env, Napi::Function callback)
  : m_reader(reader)
{
  TRACE_P("+ GetPropertyInfoWorker");
  reader->Ref();

  m_tsfn = Napi::ThreadSafeFunction::New( //
    env,
    callback,
    "GetPropertyInfoWorker",
    0,
    1,
    this,
    GetPropertyInfoWorker::Finalize,
    (void*)nullptr);

  m_thread = std::thread(&GetPropertyInfoWorker::execute, this);
}

GetPropertyInfoWorker::~GetPropertyInfoWorker()
{
  TRACE_P("- GetPropertyInfoWorker");
  auto n = m_reader->Unref();
  TRACE_P("m_ref: %u", n);
  m_thread.join();
}

void
GetPropertyInfoWorker::execute()
{
  m_info = m_reader->get_property_info();
  auto r_status = m_tsfn.BlockingCall(this, GetPropertyInfoWorker::InvokeCallback);
  TRACE_P("napi_status: %d", r_status);
  m_tsfn.Release();
}

void
GetPropertyInfoWorker::Finalize(Napi::Env, void*, GetPropertyInfoWorker* self)
{
  TRACE_ADDR(self, "[GetPropertyInfoWorker::Finalize]");
  delete self;
}

void
GetPropertyInfoWorker::InvokeCallback(Napi::Env env,
                                      Napi::Function jsCallback,
                                      GetPropertyInfoWorker* self)
{
  TRACE_ADDR(self, "[GetPropertyInfoWorker::InvokeCallback]");
  try {
    if (self->m_info) {
      auto prop_info = Napi::Object::New(env);
      auto archive_info = Napi::Array::New(env, self->m_info->archive.size());
      auto entry_info = Napi::Array::New(env, self->m_info->entry.size());
      prop_info.Set("archive", archive_info);
      prop_info.Set("entry", entry_info);

      for (uint32_t i = 0; i < self->m_info->archive.size(); i++) {
        auto& info = self->m_info->archive[i];
        auto obj = Napi::Object::New(env);
        obj.Set("propID", Napi::Number::New(env, info.pid));
        obj.Set("varType", Napi::Number::New(env, info.type));
        if (info.name) {
          obj.Set("name", ConvertBStrToNapiString(env, info.name));
        } else {
          obj.Set("name", env.Undefined());
        }
        archive_info.Set(i, obj);
      }

      for (uint32_t i = 0; i < self->m_info->entry.size(); i++) {
        auto& info = self->m_info->entry[i];
        auto obj = Napi::Object::New(env);
        obj.Set("propID", Napi::Number::New(env, info.pid));
        obj.Set("varType", Napi::Number::New(env, info.type));
        if (info.name) {
          obj.Set("name", ConvertBStrToNapiString(env, info.name));
        } else {
          obj.Set("name", env.Undefined());
        }
        entry_info.Set(i, obj);
      }

      jsCallback.Call({ OK(env, prop_info) });
    } else {
      jsCallback.Call({ ERR(env, "Failed to get property info") });
    }
  } catch (...) {
  }
}

} // namespace n7zip
