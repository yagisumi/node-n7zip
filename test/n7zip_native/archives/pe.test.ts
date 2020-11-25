import { n7zip_native } from '@/n7zip_native'
import { InStreamArg } from '@/n7zip_native_types'
import path from 'path'
import { TRACE } from '?/utils'

const baseDir = path.resolve(__dirname, '../../../ext/files/7z32')
const name = '7z.exe'
const streams: InStreamArg[] = [
  {
    type: 'path',
    source: path.join(baseDir, name),
    name,
  },
]

const all_formats = n7zip_native.getFormats()
const target_format = all_formats.filter((fmt) => fmt.name === 'PE').map((fmt) => fmt.index)
const incorrect_format = all_formats.filter((fmt) => fmt.name === 'zip').map((fmt) => fmt.index)
const format_ext = 'exe'

describe('n7zip_native/archives/pe', function () {
  test('non-existent pe file', function (this: Context, done) {
    TRACE(this)

    const r_cr = n7zip_native.createReader(
      {
        streams: [{ type: 'path', source: `non-existent.${format_ext}`, name: '' }], //
        formats: target_format,
      },
      (r_reader) => {
        process.nextTick(() => {
          expect(r_reader.error).toBeInstanceOf(Error)
          expect(r_reader.ok).toBe(false)
          expect(r_reader.error?.message).toBe(`Cannot open file: non-existent.${format_ext}`)
          done()
        })
      }
    )

    expect(r_cr.error).toBeUndefined()
    expect(r_cr.ok).toBe(true)
  })

  test('incorrect format', function (this: Context, done) {
    TRACE(this)

    const r_cr = n7zip_native.createReader(
      {
        streams, //
        formats: incorrect_format,
      },
      (r_reader) => {
        process.nextTick(() => {
          expect(r_reader.error).toBeInstanceOf(Error)
          expect(r_reader.ok).toBe(false)
          expect(r_reader.error?.message).toBe('Failed to open archive')
          done()
        })
      }
    )

    expect(r_cr.error).toBeUndefined()
    expect(r_cr.ok).toBe(true)
  })

  test('open and close', function (this: Context, done) {
    TRACE(this)

    const r_cr = n7zip_native.createReader(
      {
        streams, //
        formats: target_format,
      },
      (r_reader) => {
        process.nextTick(() => {
          expect(r_reader.error).toBeUndefined()
          expect(r_reader.ok).toBe(true)

          if (r_reader.error) {
            done()
            return
          }

          const reader = r_reader.value
          console.log(reader.getNumberOfItems())

          const r_close1 = reader.close((r_close2) => {
            process.nextTick(() => {
              expect(r_close2.error).toBeUndefined()
              expect(r_close2.ok).toBe(true)
              done()
            })
          })
          expect(r_close1.error).toBeUndefined()
          expect(r_close1.ok).toBe(true)
        })
      }
    )

    expect(r_cr.error).toBeUndefined()
    expect(r_cr.ok).toBe(true)
  })

  test('open and getPropertyInfo', function (this: Context, done) {
    TRACE(this)

    const r_cr = n7zip_native.createReader(
      {
        streams, //
        formats: target_format,
      },
      (r_reader) => {
        process.nextTick(() => {
          expect(r_reader.error).toBeUndefined()
          expect(r_reader.ok).toBe(true)

          if (r_reader.error) {
            done()
            return
          }

          const reader = r_reader.value

          const r_info1 = reader.getPropertyInfo((r_info2) => {
            process.nextTick(() => {
              console.dir(r_info2, { depth: null })

              const r_close1 = reader.close((r_close2) => {
                process.nextTick(() => {
                  expect(r_close2.error).toBeUndefined()
                  expect(r_close2.ok).toBe(true)
                  done()
                })
              })
              expect(r_close1.error).toBeUndefined()
              expect(r_close1.ok).toBe(true)
            })
          })
          console.log(r_info1)
          expect(r_info1.error).toBeUndefined()
          expect(r_info1.ok).toBe(true)
        })
      }
    )

    expect(r_cr.error).toBeUndefined()
    expect(r_cr.ok).toBe(true)
  })
})
