import fs from 'fs'
import path from 'path'
import { n7zip } from '@/n7zip'
import { n7zip_native } from '@/n7zip_native'
import { SeekOrigin, InStreamWrap } from '@/n7zip_native_types'
import { TRACE } from '?/utils'

const file = path.resolve(__dirname, '../files/in_stream.txt')
const file_c = path.resolve(__dirname, '../files/in_stream_C.txt')
const stat = fs.statSync(file)

describe('n7zip_native/in_stream', function () {
  test('FdInStream Seek Error', function (this: Context) {
    if (!n7zip.DEBUG || n7zip_native.tester == null) {
      return
    }
    TRACE(this)

    const fd = fs.openSync(file, 'r')
    const r1 = n7zip_native.tester.createInStream({ type: 'fd', source: fd, name: 'in_stream.txt' })
    expect(r1.error).toBeUndefined()
    expect(r1.ok).toBe(true)
    if (r1.ok) {
      const stream = r1.value

      fs.closeSync(fd)
      const r2 = stream.seek(5, SeekOrigin.SEEK_CUR)
      expect(r2.error).toBeInstanceOf(Error)
      expect(r2.ok).toBe(false)
    }
  })

  test('FdInStream', function (this: Context) {
    if (!n7zip.DEBUG || n7zip_native.tester == null) {
      return
    }
    TRACE(this)

    const fd = fs.openSync(file, 'r')
    const r = n7zip_native.tester.createInStream({ type: 'fd', source: fd, name: 'in_stream.txt' })
    expect(r.error).toBeUndefined()
    expect(r.ok).toBe(true)
    if (r.ok) {
      const in_stream = r.value
      testInStream(in_stream)
    }
  })

  test('FdInStream (string path)', function (this: Context) {
    if (!n7zip.DEBUG || n7zip_native.tester == null) {
      return
    }
    TRACE(this)

    const r = n7zip_native.tester.createInStream({
      type: 'path',
      source: file,
      name: 'in_stream.txt',
    })
    expect(r.error).toBeUndefined()
    expect(r.ok).toBe(true)
    if (r.ok) {
      const in_stream = r.value
      testInStream(in_stream)
    }
  })

  test('BufferInStream (ShareBuffer: false)', function (this: Context) {
    if (!n7zip.DEBUG || n7zip_native.tester == null) {
      return
    }
    TRACE(this)

    const buf = fs.readFileSync(file)

    const r = n7zip_native.tester.createInStream({
      type: 'buffer',
      source: buf,
      name: 'in_stream.txt',
      ShareBuffer: false,
    })
    expect(r.error).toBeUndefined()
    expect(r.ok).toBe(true)
    if (r.ok) {
      const in_stream = r.value
      testInStream(in_stream)

      buf.write('00000', 0, 'utf-8')
      in_stream.seek(0, SeekOrigin.SEEK_SET)

      {
        const r2 = in_stream.read(5)
        expect(r2.error).toBeUndefined()
        expect(r2.ok).toBe(true)
        if (r2.ok) {
          expect(r2.value.toString('utf-8')).toBe('AAAAA')
        }
      }
    }
  })

  test('BufferInStream (ShareBuffer: true)', function (this: Context) {
    if (!n7zip.DEBUG || n7zip_native.tester == null) {
      return
    }
    TRACE(this)

    const buf = fs.readFileSync(file)

    const r = n7zip_native.tester.createInStream({
      type: 'buffer',
      source: buf,
      name: 'in_stream.txt',
      ShareBuffer: true,
    })
    expect(r.error).toBeUndefined()
    expect(r.ok).toBe(true)
    if (r.ok) {
      const in_stream = r.value
      testInStream(in_stream)

      buf.write('00000', 0, 'utf-8')
      in_stream.seek(0, SeekOrigin.SEEK_SET)

      {
        const r2 = in_stream.read(5)
        expect(r2.error).toBeUndefined()
        expect(r2.ok).toBe(true)
        if (r2.ok) {
          expect(r2.value.toString('utf-8')).toBe('00000')
        }
      }
    }
  })

  test('MultiInStream (Buffer)', function (this: Context) {
    if (!n7zip.DEBUG || n7zip_native.tester == null) {
      return
    }
    TRACE(this)

    const buf1 = Buffer.alloc(5, 'A', 'utf-8')
    const buf2 = Buffer.alloc(5, 'B', 'utf-8')
    const buf3 = Buffer.alloc(6, 'C', 'utf-8')

    const r = n7zip_native.tester.createInStream({
      type: 'multi',
      source: [
        { type: 'buffer', source: buf1, ShareBuffer: true },
        { type: 'buffer', source: buf2, ShareBuffer: true },
        { type: 'buffer', source: buf3, ShareBuffer: true },
      ],
      name: 'in_stream.txt',
    })
    // console.log(r)
    expect(r.error).toBeUndefined()
    expect(r.ok).toBe(true)
    if (r.ok) {
      const in_stream = r.value
      testInStream(in_stream)
    }
  })

  test('MultiInStream (Buffer, file)', function (this: Context) {
    if (!n7zip.DEBUG || n7zip_native.tester == null) {
      return
    }
    TRACE(this)

    const buf1 = Buffer.alloc(5, 'A', 'utf-8')
    const buf2 = Buffer.alloc(5, 'B', 'utf-8')

    const r = n7zip_native.tester.createInStream({
      type: 'multi',
      source: [
        { type: 'buffer', source: buf1 },
        { type: 'buffer', source: buf2 },
        { type: 'path', source: file_c },
      ],
      name: 'in_stream.txt',
    })
    // console.log(r)
    expect(r.error).toBeUndefined()
    expect(r.ok).toBe(true)
    if (r.ok) {
      const in_stream = r.value
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
    const r = stream.read(15)
    expect(r.error).toBeUndefined()
    expect(r.ok).toBe(true)
    if (r.ok) {
      expect(r.value.toString('utf-8')).toBe('AAAAABBBBBCCCCC')
    }
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

  {
    const r = stream.seek(100, SeekOrigin.SEEK_END)
    expect(r.error).toBeUndefined()
    expect(r.ok).toBe(true)
    if (r.ok) {
      expect(r.value).toBe(stat.size + 100)
    }
  }

  {
    const r = stream.seek(-100, SeekOrigin.SEEK_SET)
    expect(r.error).toBeInstanceOf(Error)
    expect(r.ok).toBe(false)
  }
}
