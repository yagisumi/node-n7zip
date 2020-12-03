#pragma once

#include <Common/MyWindows.h>

#define CLSID_7ZIP(name, yy, xx) \
  DEFINE_GUID(IID_##name, 0x23170F69, 0x40C1, 0x278A, 0x00, 0x00, 0x00, yy, 0x00, xx, 0x00, 0x00)

CLSID_7ZIP(IInStream, 0x03, 0x03);
CLSID_7ZIP(ISequentialOutStream, 0x03, 0x02);

CLSID_7ZIP(ICompressCodecsInfo, 0x04, 0x60);

CLSID_7ZIP(ICryptoGetTextPassword, 0x05, 0x10);

CLSID_7ZIP(IArchiveOpenCallback, 0x06, 0x10);
CLSID_7ZIP(IArchiveOpenVolumeCallback, 0x06, 0x30);
CLSID_7ZIP(IInArchive, 0x06, 0x60);
CLSID_7ZIP(IArchiveExtractCallback, 0x06, 0x20);
