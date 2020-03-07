#pragma once

#include <Common/MyWindows.h>

#define CLSID_7ZIP(name, yy, xx) \
  DEFINE_GUID(IID_##name, 0x23170F69, 0x40C1, 0x278A, 0x00, 0x00, 0x00, yy, 0x10, xx, 0x00, 0x00)

CLSID_7ZIP(ICompressCodecsInfo, 0x04, 0x60);

#define CLSID_FORMAT(name, value) \
  DEFINE_GUID(CLSID_CFormat##name, 0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, value, 0x00, 0x00)

CLSID_FORMAT(Zip, 0x01);
// CLSID_FORMAT(BZip2, 0x02);
// CLSID_FORMAT(Rar, 0x03);
// CLSID_FORMAT(Z,   0x05);
// CLSID_FORMAT(Lzh, 0x06);
// CLSID_FORMAT(7z,  0x07);
// CLSID_FORMAT(Cab, 0x08);
// CLSID_FORMAT(Lzma,0x0A);

// CLSID_FORMAT(Wim, 0xE6);
// CLSID_FORMAT(Iso, 0xE7);
// CLSID_FORMAT(Tar, 0xEE);
// CLSID_FORMAT(GZip,0xEF);
