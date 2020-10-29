import { n7zip_native, InStreamArg } from '@/n7zip_native'
import { n7zip } from '@/n7zip'
import path from 'path'

// const file = path.resolve(__dirname, '../files/archive.zip')

describe('Reader', () => {
  const formats = n7zip_native.getFormats()

  test.only('createReader and close', (done) => {
    if (!n7zip.DEBUG || n7zip_native.tester == null) {
      return
    }

    const fmtIndices = formats.filter((fmt) => ['zip'].includes(fmt.name)).map((fmt) => fmt.index)
    const baseDir = path.resolve(__dirname, '../files') + path.sep
    const files: InStreamArg[] = [
      {
        type: 'path',
        name: 'archive.zip',
        source: path.join(baseDir, 'archive.zip'),
      },
    ]

    const r_reader = n7zip_native.createReader({
      formats: fmtIndices,
      streams: files,
      baseDir,
    })
    expect(r_reader.error).toBeUndefined()
    expect(r_reader.ok).toBe(true)
    if (r_reader.ok) {
      const reader = r_reader.value
      expect(reader.isClosed()).toBe(false)
      const r_close1 = reader.close((r) => {
        expect(r.error).toBeUndefined()
        expect(r.ok).toBe(true)
        expect(reader.isClosed()).toBe(true)
        done()
      })
      expect(r_close1.error).toBeUndefined()
      expect(r_close1.ok).toBe(true)
    }
  })

  test('createReader', () => {
    if (!n7zip.DEBUG || n7zip_native.tester == null) {
      return
    }

    // console.log(formats.filter((fmt) => ['zip', '7z', '7zip'].includes(fmt.name)))
    // console.log(formats)
    const fmtIndices = formats
      .filter((fmt) => ['zip', '7z'].includes(fmt.name))
      .map((fmt) => fmt.index)
    const files: InStreamArg[] = [
      {
        type: 'path',
        name: 'archive_rar.zip',
        source: path.resolve(__dirname, '../files/archive_rar.zip'),
      },
      // {
      //   type: 'multi',
      //   name: 'archive.7z',
      //   source: [
      //     { source: path.resolve(__dirname, '../files/archive.7z.001') },
      //     { source: path.resolve(__dirname, '../files/archive.7z.002') },
      //   ],
      // },
    ]
    // const files = [['in_stream.txt', file]]
    console.log(
      n7zip_native.createReader({
        formats: fmtIndices,
        streams: files,
        baseDir: path.dirname(path.resolve(__dirname, '../files/archive_rar.zip')) + path.sep,
      })
    )
  })
})
