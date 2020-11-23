#include "get_archive_properties_worker.h"

namespace n7zip {

GetArchivePropertiesWorker::GetArchivePropertiesWorker(Napi::Env env,
                                                       Napi::Function callback,
                                                       Reader* reader)
  : m_reader(reader)
{
  TRACE_THIS("+ GetArchivePropertiesWorker");
  auto n = m_reader->Ref();
  TRACE_THIS("m_reader->Ref(): %u", n);

  m_tsfn = Napi::ThreadSafeFunction::New( //
    env,
    callback,
    "GetArchivePropertiesWorker",
    0,
    1,
    this,
    GetArchivePropertiesWorker::Finalize,
    (void*)nullptr);

  m_thread = std::thread(&GetArchivePropertiesWorker::execute, this);
}

GetArchivePropertiesWorker::~GetArchivePropertiesWorker()
{
  TRACE_THIS("- GetArchivePropertiesWorker");
  auto n = m_reader->Unref();
  TRACE_THIS("m_reader->Unref(): %u", n);
  m_thread.join();
}

void
GetArchivePropertiesWorker::execute()
{
  m_result = m_reader->get_archive_properties();
  auto r_status = m_tsfn.BlockingCall(this, GetArchivePropertiesWorker::InvokeCallback);
  TRACE_THIS("napi_status: %d", r_status);
  m_tsfn.Release();
}

void
GetArchivePropertiesWorker::Finalize(Napi::Env, void*, GetArchivePropertiesWorker* self)
{
  TRACE_PTR(self, "[GetArchivePropertiesWorker::Finalize]");
  delete self;
}

void
GetArchivePropertiesWorker::InvokeCallback(Napi::Env env,
                                           Napi::Function jsCallback,
                                           GetArchivePropertiesWorker* self)
{
  TRACE_PTR(self, "[GetArchivePropertiesWorker::InvokeCallback]");
  try {
    auto length = self->m_result.size();
    if (length == self->m_reader->m_num_of_arc_props) {
      auto prop_ary = Napi::Array::New(env, length);
      for (size_t i = 0; i < length; i++) {
        auto& prop = self->m_result[i];
        auto prop_obj = Napi::Object::New(env);
        prop_obj.Set("id", Napi::Number::New(env, prop.prop_id));
        prop_obj.Set("value", ConvertPropVariant(env, prop.prop));
        prop_ary.Set(i, prop_obj);
      }
      jsCallback.Call({ OK(env, prop_ary) });
    } else {
      jsCallback.Call({ ERR(env, "Failed to get_archive_properties") });
    }
  } catch (...) {
  }
}

} // namespace n7zip
