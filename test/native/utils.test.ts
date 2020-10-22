import { n7zip_native } from '@/n7zip_native'
import { Buffer } from 'buffer'

describe('utils', () => {
  test('inspectUString', () => {
    if (!n7zip_native.DEBUG || n7zip_native.tester == null) {
      return
    }

    const char = '𡈽' // U+2123D
    // utf-8 [0xF0, 0xA1, 0x88, 0xBD]
    // utf-16le [0x44, 0xd8, 0x3d, 0xde]
    // utf-32le [0x3d, 0x12, 0x02, 0x00]
    const r = n7zip_native.tester.inspectUString(char)
    expect(r).not.toBeUndefined()
    if (r === undefined) {
      return
    }

    if (process.platform === 'win32') {
      // utf-16le (wchar_t: 2byte)
      const expected = Buffer.from([0x44, 0xd8, 0x3d, 0xde])
      expect(Buffer.compare(r, expected)).toBe(0)
    } else {
      // utf-16le (wchar_t: 4byte)
      const expected = Buffer.from([0x44, 0xd8, 0x00, 0x00, 0x3d, 0xde, 0x00, 0x00])
      expect(Buffer.compare(r, expected)).toBe(0)
    }
  })
})
