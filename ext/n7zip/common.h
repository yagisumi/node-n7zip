#pragma once

#ifdef _WIN32
  #define _WINSOCKAPI_
#else
  #include <include_windows/windows.h>
  #include <myWindows/StdAfx.h>
#endif

#include <Common/MyWindows.h>

#include <7zip/Archive/IArchive.h>
#include <7zip/IStream.h>
#include <7zip/ICoder.h>
#include <7zip/Common/MethodId.h>
#include <Common/MyString.h>
#include <Common/UTFConvert.h>
#include <Windows/PropVariant.h>

#include <memory>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <utility>
#include <set>

#define NAPI_VERSION 4
#include <napi.h>

#include <n7zip/my_com.h>
// #include <Common/MyCom.h>

#include "debug.h"
#include "guid.h"
#include "utils.h"
#include "library.h"
