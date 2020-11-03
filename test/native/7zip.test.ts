import { n7zip_native } from '@/n7zip_native'
import { Buffer } from 'buffer'

describe('7zip', () => {
  test('UString is utf-16le encoding, regardless of the size of wchar_t.', () => {
    if (!n7zip_native.DEBUG || n7zip_native.tester == null) {
      return
    }

    const char = '𡈽' // U+2123D
    // utf-8 [0xF0, 0xA1, 0x88, 0xBD]
    // utf-16le [0x44, 0xd8, 0x3d, 0xde]
    // utf-32le [0x3d, 0x12, 0x02, 0x00]
    const r1 = n7zip_native.tester.inspectUString(char)
    expect(r1).not.toBeUndefined()
    if (r1 === undefined) {
      return
    }

    if (process.platform === 'win32') {
      // utf-16le (wchar_t: 2byte)
      const expected = Buffer.from([0x44, 0xd8, 0x3d, 0xde])
      expect(Buffer.compare(r1, expected)).toBe(0)
    } else {
      // utf-16le (wchar_t: 4byte)
      const expected = Buffer.from([0x44, 0xd8, 0x00, 0x00, 0x3d, 0xde, 0x00, 0x00])
      expect(Buffer.compare(r1, expected)).toBe(0)
    }

    const r2 = n7zip_native.tester.convertUStringToAString(char)
    expect(r2).not.toBeUndefined()
    if (r2 === undefined) {
      return
    }
    const expected2 = Buffer.from([0xf0, 0xa1, 0x88, 0xbd])
    expect(Buffer.compare(r2, expected2)).toBe(0)
  })

  test('value.IsObject(null) => false', () => {
    if (!n7zip_native.DEBUG || n7zip_native.tester == null) {
      return
    }

    expect(n7zip_native.tester.isObject(null)).toBe(false)
    expect(n7zip_native.tester.isObject([])).toBe(true)
    expect(n7zip_native.tester.isObject({})).toBe(true)

    expect(n7zip_native.tester.isObject(undefined)).toBe(false)
    expect(n7zip_native.tester.isObject('')).toBe(false)
    expect(n7zip_native.tester.isObject(100)).toBe(false)
  })
})
