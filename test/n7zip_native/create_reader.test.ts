import { n7zip_native } from '@/n7zip_native'
import { InStreamArg } from '@/n7zip_native_types'
import path from 'path'
import { TRACE } from '?/utils'

describe('n7zip_native/create_reader', function () {
  const formats = n7zip_native.getFormats()

  test('createReader', function (this: Context, done) {
    TRACE(this)

    const fmtIndices = formats.filter((fmt) => ['zip'].includes(fmt.name)).map((fmt) => fmt.index)
    const baseDir = path.resolve(__dirname, '../files') + path.sep
    const files: InStreamArg[] = [
      {
        type: 'path',
        name: 'archive.zip',
        source: path.join(baseDir, 'archive.zip'),
      },
    ]

    const r_create_reader = n7zip_native.createReader(
      {
        formats: fmtIndices,
        streams: files,
        baseDir,
      },
      (r) => {
        expect(r.error).toBeUndefined()
        expect(r.ok).toBe(true)
        if (r.ok) {
          const reader = r.value

          expect(reader.numberOfEntries).toBe(29)

          if (process.platform === 'win32') {
            expect(reader.numberOfArchiveProperties).toBe(8)
          } else {
            expect(reader.numberOfArchiveProperties).toBe(7)
          }

          if (process.platform === 'win32') {
            expect(reader.numberOfProperties).toBe(17)
          } else {
            expect(reader.numberOfProperties).toBe(15)
          }

          expect(reader.isClosed()).toBe(false)

          const r_close1 = reader.close((r_close2) => {
            expect(r_close2.error).toBeUndefined()
            expect(r_close2.ok).toBe(true)
            expect(reader.isClosed()).toBe(true)
            done()
          })
          expect(r_close1.error).toBeUndefined()
          expect(r_close1.ok).toBe(true)
        }
      }
    )

    expect(r_create_reader.error).toBeUndefined()
    expect(r_create_reader.ok).toBe(true)
  })

  describe('errors', function () {
    test('argument errors', function (this: Context) {
      TRACE(this)

      {
        // The first argument must be Object
        const r = n7zip_native.createReader(null as any, () => {})
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('The first argument must be Object')
      }

      {
        // The first argument must be Object
        const r = n7zip_native.createReader([] as any, () => {})
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('The first argument must be Object')
      }

      {
        // The second argument must be callback function
        const r = (n7zip_native['createReader'] as any)({})
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('The second argument must be callback function')
      }

      {
        // The second argument must be callback function
        const r = (n7zip_native['createReader'] as any)({}, 'test')
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('The second argument must be callback function')
      }
    })

    test('invalid formats', function (this: Context) {
      TRACE(this)

      {
        // 'formats' must be Array
        const r = n7zip_native.createReader({} as any, () => {})
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe("'formats' must be Array")
      }

      {
        // 'formats' must be Array
        const r = n7zip_native.createReader({} as any, () => {})
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe("'formats' must be Array")
      }

      {
        // No valid value for 'formats'
        const r = n7zip_native.createReader({ formats: [] } as any, () => {})
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe("No valid value for 'formats'")
      }
    })

    test('invalid streams', function (this: Context) {
      TRACE(this)

      {
        // 'streams' must be Array
        const r = n7zip_native.createReader({ formats: [0] } as any, () => {})
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe("'streams' must be Array")
      }

      {
        // 'streams' is empty
        const r = n7zip_native.createReader({ formats: [0], streams: [] } as any, () => {})
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe("'streams' is empty")
      }

      {
        // No stream type is specified (at streams[0])
        const r = n7zip_native.createReader({ formats: [0], streams: [{}] } as any, () => {})
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('No stream type is specified (at streams[0])')
      }

      {
        // Invalid stream type (at streams[0])
        const r = n7zip_native.createReader({ formats: [0], streams: [[]] } as any, () => {})
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('Invalid stream type (at streams[0])')
      }

      {
        // Unkonwn type: test (at streams[0])
        const r = n7zip_native.createReader(
          { formats: [0], streams: [{ type: 'test' }] } as any,
          () => {}
        )
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('Unkonwn type: test (at streams[0])')
      }

      {
        // No stream type is specified (at streams[1])
        const r = n7zip_native.createReader(
          { formats: [0], streams: [{ type: 'fd', source: 0 }, {}] } as any,
          () => {}
        )
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('No stream type is specified (at streams[1])')
      }
    })

    test('invalid fd stream', function (this: Context) {
      TRACE(this)

      {
        // The source of fd type must be Number (at streams[0])
        const r = n7zip_native.createReader(
          { formats: [0], streams: [{ type: 'fd' }] } as any,
          () => {}
        )
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('The source of fd type must be Number (at streams[0])')
      }

      {
        // The source of fd type must be Number (at streams[0])
        const r = n7zip_native.createReader(
          { formats: [0], streams: [{ type: 'fd', source: '100' }] } as any,
          () => {}
        )
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('The source of fd type must be Number (at streams[0])')
      }
    })

    test('invalid path stream', function (this: Context) {
      TRACE(this)

      {
        // The source of path type must be String (at streams[0])
        const r = n7zip_native.createReader(
          { formats: [0], streams: [{ type: 'path' }] } as any,
          () => {}
        )
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('The source of path type must be String (at streams[0])')
      }

      {
        // The source of path type must be String (at streams[0])
        const r = n7zip_native.createReader(
          { formats: [0], streams: [{ type: 'path', source: 100 }] } as any,
          () => {}
        )
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('The source of path type must be String (at streams[0])')
      }
    })

    test('invalid buffer stream', function (this: Context) {
      TRACE(this)

      {
        // The source of buffer type must be Buffer (at streams[0])
        const r = n7zip_native.createReader(
          { formats: [0], streams: [{ type: 'buffer' }] } as any,
          () => {}
        )
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('The source of buffer type must be Buffer (at streams[0])')
      }

      {
        // The source of buffer type must be Buffer (at streams[0])
        const r = n7zip_native.createReader(
          { formats: [0], streams: [{ type: 'buffer', source: '100' }] } as any,
          () => {}
        )
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('The source of buffer type must be Buffer (at streams[0])')
      }
    })

    test('multi buffer stream', function (this: Context) {
      TRACE(this)

      {
        // The source of multi type must be Array (at streams[0])
        const r = n7zip_native.createReader(
          { formats: [0], streams: [{ type: 'multi' }] } as any,
          () => {}
        )
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('The source of multi type must be Array (at streams[0])')
      }

      {
        // The source of multi type must be Array (at streams[0])
        const r = n7zip_native.createReader(
          { formats: [0], streams: [{ type: 'multi', source: '100' }] } as any,
          () => {}
        )
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('The source of multi type must be Array (at streams[0])')
      }

      {
        // 'source' is empty (at streams[0])
        const r = n7zip_native.createReader(
          { formats: [0], streams: [{ type: 'multi', source: [] }] } as any,
          () => {}
        )
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe("'source' is empty (at streams[0])")
      }

      {
        // Unkonwn type: test (at source[0]) (at streams[0])
        const r = n7zip_native.createReader(
          { formats: [0], streams: [{ type: 'multi', source: [{ type: 'test' }] }] } as any,
          () => {}
        )
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('Unkonwn type: test (at source[0]) (at streams[0])')
      }

      {
        // Multi type in multi type is not supported (at source[0]) (at streams[0])
        const r = n7zip_native.createReader(
          { formats: [0], streams: [{ type: 'multi', source: [{ type: 'multi' }] }] } as any,
          () => {}
        )
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe(
          'Multi type in multi type is not supported (at source[0]) (at streams[0])'
        )
      }

      {
        // No stream type is specified (at source[1]) (at streams[0])
        const r = n7zip_native.createReader(
          {
            formats: [0],
            streams: [{ type: 'multi', source: [{ type: 'fd', source: 0 }, {}] }],
          } as any,
          () => {}
        )
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('No stream type is specified (at source[1]) (at streams[0])')
      }
    })
  })
})
