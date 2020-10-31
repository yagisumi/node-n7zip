#include "library.h"

namespace n7zip {

Library::Library(TLIB lib)
  : m_lib(lib)
{
  TRACE("+ Library::m_lib %p", m_lib);

  this->CreateObject = MyGetProcAddress<Func_CreateObject>(m_lib, "CreateObject");
  GetNumberOfFormats = MyGetProcAddress<Func_GetNumberOfFormats>(m_lib, "GetNumberOfFormats");
  GetHandlerProperty2 = MyGetProcAddress<Func_GetHandlerProperty2>(m_lib, "GetHandlerProperty2");
  GetNumberOfMethods = MyGetProcAddress<Func_GetNumberOfMethods>(m_lib, "GetNumberOfMethods");
  GetMethodProperty = MyGetProcAddress<Func_GetMethodProperty>(m_lib, "GetMethodProperty");
  CreateDecoder = MyGetProcAddress<Func_CreateDecoder>(m_lib, "CreateDecoder");
  CreateEncoder = MyGetProcAddress<Func_CreateEncoder>(m_lib, "CreateEncoder");
  SetCodecs = MyGetProcAddress<Func_SetCodecs>(m_lib, "SetCodecs");

  if (GetNumberOfFormats) {
    auto r = GetNumberOfFormats(&m_num_formats);
    if (r != S_OK) {
      m_num_formats = 0;
    }
  }

  if (GetNumberOfMethods) {
    auto r = GetNumberOfMethods(&m_num_methods);
    if (r != S_OK) {
      m_num_methods = 0;
    }
  }
}

Library::~Library()
{
  if (m_lib != NULL) {
#ifdef _WIN32
    FreeLibrary(m_lib);
#else
    dlclose(m_lib);
#endif
    TRACE("- Library::m_lib %p", m_lib);
    m_lib = NULL;
  }
}

CMyComPtr<LibraryInfo> g_library_info(new LibraryInfo);

std::shared_lock<std::shared_timed_mutex>
LibraryInfo::get_shared_lock()
{
  return std::shared_lock<std::shared_timed_mutex>(m_external_mutex);
}

std::unique_lock<std::shared_timed_mutex>
LibraryInfo::get_deferred_unique_lock()
{
  return std::unique_lock<std::shared_timed_mutex>(m_external_mutex, std::defer_lock);
}

HRESULT
LibraryInfo::add_library(Napi::String& path)
{
  TLIB lib;
#ifdef _WIN32
  auto path_str = path.Utf16Value();
  lib = LoadLibraryW(reinterpret_cast<const wchar_t*>(path_str.c_str()));
#else
  auto path_str = path.Utf8Value();
  lib = dlopen(path_str.c_str(), RTLD_LAZY);
#endif
  if (lib == NULL) {
    return E_FAIL;
  }

  std::unique_ptr<Library> library(new Library(lib));
  if (!library->has_formats() && !library->has_methods()) {
    return E_FAIL;
  }

  auto start_num_formats = m_formats.size();
  auto start_num_methods = m_methods.size();

  load_formats(library);
  load_methods(library);

  if (m_formats.size() > start_num_formats || m_methods.size() > start_num_methods) {
    m_libraries.push_back(std::move(library));
    if (m_libraries.size() > 1) {
      for (size_t i = 0; i < m_libraries.size(); i++) {
        if (m_libraries.at(i)->SetCodecs) {
          m_libraries.at(i)->SetCodecs(this);
        }
      }
    }
    return S_OK;
  } else {
    return S_FALSE;
  }
}

void
LibraryInfo::load_formats(std::unique_ptr<Library>& library)
{
  if (!library->has_formats()) {
    return;
  }

  auto lib_index = m_libraries.size();

  for (UInt32 i = 0; i < library->num_of_formats(); i++) {
    NWindows::NCOM::CPropVariant prop;
    std::unique_ptr<Format> format(new Format);

    auto r_id = library->GetHandlerProperty2(i, NArchive::NHandlerPropID::kClassID, &prop);
    if (r_id != S_OK || prop.vt != VT_BSTR) {
      continue;
    }

    if (::SysStringByteLen(prop.bstrVal) != sizeof(GUID)) {
      continue;
    }

    format->ClassId = *(GUID*)prop.bstrVal;

    if (m_format_ids.count(format->ClassId) > 0) {
      continue;
    }

    prop.Clear();
    auto r_name = library->GetHandlerProperty2(i, NArchive::NHandlerPropID::kName, &prop);
    if (r_name == S_OK && prop.vt == VT_BSTR && prop.bstrVal) {
      UString ustr(prop.bstrVal);
      AString astr;
      ConvertUnicodeToUTF8(ustr, astr);
      format->Name.append(astr.Ptr());
    }

    prop.Clear();
    auto r_ext = library->GetHandlerProperty2(i, NArchive::NHandlerPropID::kExtension, &prop);
    if (r_ext == S_OK && prop.vt == VT_BSTR) {
      UString ustr(prop.bstrVal);
      AString astr;
      ConvertUnicodeToUTF8(ustr, astr);
      format->Extension.append(astr.Ptr());
    }

    prop.Clear();
    auto r_addext = library->GetHandlerProperty2(i, NArchive::NHandlerPropID::kAddExtension, &prop);
    if (r_addext == S_OK && prop.vt == VT_BSTR) {
      UString ustr(prop.bstrVal);
      AString astr;
      ConvertUnicodeToUTF8(ustr, astr);
      format->AddExtension.append(astr.Ptr());
    }

    prop.Clear();
    auto r_update = library->GetHandlerProperty2(i, NArchive::NHandlerPropID::kUpdate, &prop);
    if (r_update == S_OK && prop.vt == VT_BOOL) {
      format->CanUpdate = (prop.boolVal != 0);
    }

    prop.Clear();
    auto r_flags = library->GetHandlerProperty2(i, NArchive::NHandlerPropID::kFlags, &prop);
    if (r_flags == S_OK && prop.vt == VT_UI4) {
      format->Flags = prop.uintVal;
    }

    format->Index = m_formats.size();
    format->LibIndex = lib_index;
    format->LibFmtIndex = i;
    m_format_ids.insert(format->ClassId);
    m_formats.push_back(std::move(format));
  }
}

void
LibraryInfo::load_methods(std::unique_ptr<Library>& library)
{
  if (!library->has_methods()) {
    return;
  }

  auto lib_index = m_libraries.size();

  for (UInt32 i = 0; i < library->num_of_methods(); i++) {
    NWindows::NCOM::CPropVariant prop;
    std::unique_ptr<Method> method(new Method);

    auto r_id = library->GetMethodProperty(i, NMethodPropID::kID, &prop);
    if (r_id != S_OK || prop.vt != VT_UI8) {
      continue;
    }

    method->Id = prop.uhVal.QuadPart;
    if (m_method_ids.count(method->Id) > 0) {
      continue;
    }

    prop.Clear();
    auto r_name = library->GetMethodProperty(i, NMethodPropID::kName, &prop);
    if (r_name == S_OK && prop.vt == VT_BSTR) {
      UString ustr(prop.bstrVal);
      AString astr;
      ConvertUnicodeToUTF8(ustr, astr);
      method->Name.append(astr.Ptr());
    }

    prop.Clear();
    auto r_decoder = library->GetMethodProperty(i, NMethodPropID::kDecoderIsAssigned, &prop);
    if (r_decoder == S_OK) {
      method->DecoderIsAssigned = prop.boolVal;
    }

    prop.Clear();
    auto r_encoder = library->GetMethodProperty(i, NMethodPropID::kEncoderIsAssigned, &prop);
    if (r_encoder == S_OK) {
      method->EncoderIsAssigned = prop.boolVal;
    }

    method->Index = m_methods.size();
    method->LibIndex = lib_index;
    method->LibMtdIndex = i;
    m_method_ids.insert(method->Id);
    m_methods.push_back(std::move(method));
  }
}

STDMETHODIMP
LibraryInfo::GetNumMethods(UInt32* numMethods)
{
  if (numMethods) {
    *numMethods = m_methods.size();
  }
  return S_OK;
}

STDMETHODIMP
LibraryInfo::GetProperty(UInt32 index, PROPID propID, PROPVARIANT* value)
{
  if (m_methods.size() <= index) {
    return E_FAIL;
  }

  auto& method = m_methods.at(index);
  auto& library = m_libraries.at(method->LibIndex);
  return library->GetMethodProperty(method->LibMtdIndex, propID, value);
}

STDMETHODIMP
LibraryInfo::CreateDecoder(UInt32 index, const GUID* iid, void** coder)
{
  if (m_methods.size() <= index) {
    return E_FAIL;
  }

  auto& method = m_methods.at(index);
  auto& library = m_libraries.at(method->LibIndex);
  return library->CreateDecoder(method->LibMtdIndex, iid, coder);
}

STDMETHODIMP
LibraryInfo::CreateEncoder(UInt32 index, const GUID* iid, void** coder)
{
  if (m_methods.size() <= index) {
    return E_FAIL;
  }

  auto& method = m_methods.at(index);
  auto& library = m_libraries.at(method->LibIndex);
  return library->CreateEncoder(method->LibMtdIndex, iid, coder);
}

Napi::Array
LibraryInfo::get_formats(const Napi::CallbackInfo& info)
{
  auto env = info.Env();
  auto ary = Napi::Array::New(env, m_formats.size());

  for (size_t i = 0; i < m_formats.size(); i++) {
    auto obj = Napi::Object::New(env);
    auto& format = m_formats.at(i);
    obj.Set("index", format->Index);
    obj.Set("name", format->Name.c_str());
    obj.Set("extension", format->Extension.c_str());
    obj.Set("addExtension", format->AddExtension.c_str());
    obj.Set("canUpdate", format->CanUpdate);
    auto guid = GuidToString(&(format->ClassId));
    obj.Set("classId", guid.c_str());

    auto flags = Napi::Object::New(env);
    flags.Set("KeepName", format->KeepName());
    flags.Set("FindSignature", format->FindSignature());
    flags.Set("AltStreams", format->AltStreams());
    flags.Set("NtSecure", format->NtSecure());
    flags.Set("SymLinks", format->SymLinks());
    flags.Set("HardLinks", format->HardLinks());
    flags.Set("UseGlobalOffset", format->UseGlobalOffset());
    flags.Set("StartOpen", format->StartOpen());
    flags.Set("BackwardOpen", format->BackwardOpen());
    flags.Set("PreArc", format->PreArc());
    flags.Set("PureStartOpen", format->PureStartOpen());
    obj.Set("flags", flags);

    ary.Set(i, obj);
  }

  return ary;
}

Napi::Array
LibraryInfo::get_codecs(const Napi::CallbackInfo& info)
{
  auto env = info.Env();
  auto ary = Napi::Array::New(env, m_methods.size());

  for (size_t i = 0; i < m_methods.size(); i++) {
    auto obj = Napi::Object::New(env);
    auto& method = m_methods.at(i);
    obj.Set("name", Napi::String::New(env, method->Name.c_str()));
    obj.Set("index", Napi::Number::New(env, method->Index));
    obj.Set("id", Napi::Number::New(env, method->Id));
    ary.Set(i, obj);
  }

  return ary;
}

size_t
LibraryInfo::get_formats_length()
{
  return m_formats.size();
}

const std::unique_ptr<Format>&
LibraryInfo::get_format(size_t idx)
{
  static std::unique_ptr<Format> out_of_range;
  if (idx < m_formats.size()) {
    return m_formats.at(idx);
  } else {
    return out_of_range;
  }
}

HRESULT
LibraryInfo::create_object(size_t fmt_index, const GUID* iid, void** outObject)
{
  if (fmt_index < m_formats.size()) {
    auto& fmt = m_formats.at(fmt_index);
    auto& lib = m_libraries.at(fmt->LibIndex);

    TRACE("fmt.ClassId: %s", GuidToString(&fmt->ClassId).c_str());

    return lib->CreateObject(&fmt->ClassId, iid, outObject);
  } else {
    return E_INVALIDARG;
  }
}

static Napi::Object
loadLibrary(const Napi::CallbackInfo& info)
{
  auto env = info.Env();
  if (info.Length() == 0 || !info[0].IsString()) {
    return ERR(env, "loadLibrary() argument must be a string");
  }

  auto lock = g_library_info->get_deferred_unique_lock();
  if (!lock.try_lock()) {
    return ERR(env, "Failed to lock");
  }

  auto path = info[0].As<Napi::String>();
  auto r = g_library_info->add_library(path);
  if (SUCCEEDED(r)) {
    return OK(env, Napi::Boolean::New(env, r == S_OK));
  } else {
    return ERR(env, "Failed to load library");
  }
}

static Napi::Array
getFormats(const Napi::CallbackInfo& info)
{
  auto lock = g_library_info->get_shared_lock();
  return g_library_info->get_formats(info);
}

static Napi::Array
getCodecs(const Napi::CallbackInfo& info)
{
  auto lock = g_library_info->get_shared_lock();
  return g_library_info->get_codecs(info);
}

Napi::Object
InitLibrary(Napi::Env env, Napi::Object exports)
{
  exports.Set("loadLibrary", Napi::Function::New(env, loadLibrary));
  exports.Set("getFormats", Napi::Function::New(env, getFormats));
  exports.Set("getCodecs", Napi::Function::New(env, getCodecs));

  return exports;
}

} // namespace n7zip
