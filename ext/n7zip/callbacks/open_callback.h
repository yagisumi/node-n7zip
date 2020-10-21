#pragma once

#include "n7zip/common.h"

class OpenCallback
  : public IArchiveOpenCallback
  , public IArchiveOpenVolumeCallback
  , public ICryptoGetTextPassword
  , public CMyUnknownImp
{};
