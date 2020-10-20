import fs from 'fs'
import path from 'path'
import { n7zip } from '@/n7zip'
import { n7zip_native, SeekOrigin, InStreamWrap } from '@/n7zip_native'

const file = path.join(__dirname, 'files/in_stream.txt')
const stat = fs.statSync(file)

describe('InStream', () => {
  test('FdInStream Seek Error', () => {
    if (!n7zip.DEBUG || n7zip_native.tester == null) {
      return
    }

    const r1 = n7zip_native.tester.createInStream(0)
    expect(r1.error).toBeUndefined()
    expect(r1.ok).toBe(true)
    if (r1.ok) {
      const stream = r1.value
      const r2 = stream.seek(5, SeekOrigin.SEEK_CUR)
      expect(r2.error).toBeInstanceOf(Error)
      expect(r2.ok).toBe(false)
    }
  })

  test('FdInStream', () => {
    if (!n7zip.DEBUG || n7zip_native.tester == null) {
      return
    }

    const fd = fs.openSync(file, 'r')
    const r_in_stream = n7zip_native.tester.createInStream(fd, true)
    expect(r_in_stream.error).toBeUndefined()
    expect(r_in_stream.ok).toBe(true)
    if (r_in_stream.ok) {
      const in_stream = r_in_stream.value
      testInStream(in_stream)
    }
  })
})

function testInStream(stream: InStreamWrap) {
  {
    const r = stream.seek(0, SeekOrigin.SEEK_END)
    expect(r.error).toBeUndefined()
    expect(r.ok).toBe(true)
    expect(r.value).toBe(stat.size)
  }

  {
    const r = stream.seek(0, SeekOrigin.SEEK_SET)
    expect(r.error).toBeUndefined()
    expect(r.ok).toBe(true)
    expect(r.value).toBe(0)
  }

  {
    const r = stream.read(5)
    expect(r.error).toBeUndefined()
    expect(r.ok).toBe(true)
    if (r.ok) {
      expect(r.value.toString('utf-8')).toBe('AAAAA')
    }
  }

  {
    const r = stream.seek(5, SeekOrigin.SEEK_CUR)
    expect(r.error).toBeUndefined()
    expect(r.ok).toBe(true)
    expect(r.value).toBe(10)
  }

  {
    const r = stream.read(5)
    expect(r.error).toBeUndefined()
    expect(r.ok).toBe(true)
    if (r.ok) {
      expect(r.value.toString('utf-8')).toBe('CCCCC')
    }
  }
}
