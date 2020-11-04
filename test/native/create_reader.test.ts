import { n7zip_native } from '@/n7zip_native'
import path from 'path'

describe('create_reader', () => {
  describe.only('check errors', () => {
    test('argument errors', () => {
      {
        // The first argument must be Object
        const r = n7zip_native.createReader2(null as any, () => {})
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('The first argument must be Object')
      }

      {
        // The first argument must be Object
        const r = n7zip_native.createReader2([] as any, () => {})
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('The first argument must be Object')
      }

      {
        // The second argument must be callback function
        const r = (n7zip_native['createReader2'] as any)({})
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('The second argument must be callback function')
      }

      {
        // The second argument must be callback function
        const r = (n7zip_native['createReader2'] as any)({}, 'test')
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('The second argument must be callback function')
      }
    })

    test('invalid formats', () => {
      {
        // 'formats' must be Array
        const r = n7zip_native.createReader2({} as any, () => {})
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe("'formats' must be Array")
      }

      {
        // 'formats' must be Array
        const r = n7zip_native.createReader2({} as any, () => {})
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe("'formats' must be Array")
      }

      {
        // No valid value for 'formats'
        const r = n7zip_native.createReader2({ formats: [] } as any, () => {})
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe("No valid value for 'formats'")
      }
    })

    test('invalid streams', () => {
      {
        // 'streams' must be Array
        const r = n7zip_native.createReader2({ formats: [0] } as any, () => {})
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe("'streams' must be Array")
      }

      {
        // 'streams' is empty
        const r = n7zip_native.createReader2({ formats: [0], streams: [] } as any, () => {})
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe("'streams' is empty")
      }

      {
        // No stream type is specified (at streams[0])
        const r = n7zip_native.createReader2({ formats: [0], streams: [{}] } as any, () => {})
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('No stream type is specified (at streams[0])')
      }

      {
        // Invalid stream type (at streams[0])
        const r = n7zip_native.createReader2({ formats: [0], streams: [[]] } as any, () => {})
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('Invalid stream type (at streams[0])')
      }

      {
        // Unkonwn type: test (at streams[0])
        const r = n7zip_native.createReader2(
          { formats: [0], streams: [{ type: 'test' }] } as any,
          () => {}
        )
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('Unkonwn type: test (at streams[0])')
      }

      {
        // No stream type is specified (at streams[1])
        const r = n7zip_native.createReader2(
          { formats: [0], streams: [{ type: 'fd', source: 0 }, {}] } as any,
          () => {}
        )
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('No stream type is specified (at streams[1])')
      }
    })

    test('invalid fd stream', () => {
      {
        // The source of fd type must be Number (at streams[0])
        const r = n7zip_native.createReader2(
          { formats: [0], streams: [{ type: 'fd' }] } as any,
          () => {}
        )
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('The source of fd type must be Number (at streams[0])')
      }

      {
        // The source of fd type must be Number (at streams[0])
        const r = n7zip_native.createReader2(
          { formats: [0], streams: [{ type: 'fd', source: '100' }] } as any,
          () => {}
        )
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('The source of fd type must be Number (at streams[0])')
      }
    })

    test('invalid path stream', () => {
      {
        // The source of path type must be String (at streams[0])
        const r = n7zip_native.createReader2(
          { formats: [0], streams: [{ type: 'path' }] } as any,
          () => {}
        )
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('The source of path type must be String (at streams[0])')
      }

      {
        // The source of path type must be String (at streams[0])
        const r = n7zip_native.createReader2(
          { formats: [0], streams: [{ type: 'path', source: 100 }] } as any,
          () => {}
        )
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('The source of path type must be String (at streams[0])')
      }
    })

    test('invalid buffer stream', () => {
      {
        // The source of buffer type must be Buffer (at streams[0])
        const r = n7zip_native.createReader2(
          { formats: [0], streams: [{ type: 'buffer' }] } as any,
          () => {}
        )
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('The source of buffer type must be Buffer (at streams[0])')
      }

      {
        // The source of buffer type must be Buffer (at streams[0])
        const r = n7zip_native.createReader2(
          { formats: [0], streams: [{ type: 'buffer', source: '100' }] } as any,
          () => {}
        )
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('The source of buffer type must be Buffer (at streams[0])')
      }
    })

    test('multi buffer stream', () => {
      {
        // The source of multi type must be Array (at streams[0])
        const r = n7zip_native.createReader2(
          { formats: [0], streams: [{ type: 'multi' }] } as any,
          () => {}
        )
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('The source of multi type must be Array (at streams[0])')
      }

      {
        // The source of multi type must be Array (at streams[0])
        const r = n7zip_native.createReader2(
          { formats: [0], streams: [{ type: 'multi', source: '100' }] } as any,
          () => {}
        )
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('The source of multi type must be Array (at streams[0])')
      }

      {
        // 'source' is empty (at streams[0])
        const r = n7zip_native.createReader2(
          { formats: [0], streams: [{ type: 'multi', source: [] }] } as any,
          () => {}
        )
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe("'source' is empty (at streams[0])")
      }

      {
        // Unkonwn type: test (at source[0]) (at streams[0])
        const r = n7zip_native.createReader2(
          { formats: [0], streams: [{ type: 'multi', source: [{ type: 'test' }] }] } as any,
          () => {}
        )
        expect(r.error).toBeInstanceOf(TypeError)
        expect(r.ok).toBe(false)
        expect(r.error?.message).toBe('Unkonwn type: test (at source[0]) (at streams[0])')
      }

      {
        // Multi type in multi type is not supported (at source[0]) (at streams[0])
        const r = n7zip_native.createReader2(
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
        const r = n7zip_native.createReader2(
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
