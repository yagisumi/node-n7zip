import { n7zip_native } from '@/n7zip_native'
import { InStreamArg } from '@/n7zip_native_types'
import path from 'path'
import { TRACE } from '?/utils'

const baseDir = path.resolve(__dirname, '../../files')
const name = 'archive.zip'
const num_of_items = 29
const streams: InStreamArg[] = [
  {
    type: 'path',
    source: path.join(baseDir, name),
    name,
  },
]

const fmt_name = 'zip'
const all_formats = n7zip_native.getFormats()
const target_format = all_formats.filter((fmt) => fmt.name === fmt_name).map((fmt) => fmt.index)
const incorrect_format = all_formats.filter((fmt) => fmt.name === '7z').map((fmt) => fmt.index)
const format_ext = 'zip'

describe('n7zip_native/archives/zip', function () {
  test('non-existent zip file', function (this: Context, done) {
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
          expect(reader.numberOfEntries).toBe(num_of_items)
          expect(reader.formatName).toBe(fmt_name)
          expect(reader.formatIndex).toBe(target_format[0])

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

  test('getPropertyInfo', function (this: Context, done) {
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
          expect(reader.numberOfEntries).toBe(num_of_items)

          const result1 = reader.getPropertyInfo((result2) => {
            process.nextTick(() => {
              console.dir(result2, { depth: null })

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
          console.log(result1)
          expect(result1.error).toBeUndefined()
          expect(result1.ok).toBe(true)
        })
      }
    )

    expect(r_cr.error).toBeUndefined()
    expect(r_cr.ok).toBe(true)
  })

  test('getEntries', function (this: Context, done) {
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
          expect(reader.numberOfEntries).toBe(num_of_items)
          console.log({ index: reader.formatIndex, name: reader.formatName })

          const result1 = reader.getEntries({}, (result2) => {
            process.nextTick(() => {
              console.dir(result2, { depth: null })

              if (result2.error || result2.value.done) {
                const r_close1 = reader.close((r_close2) => {
                  process.nextTick(() => {
                    expect(r_close2.error).toBeUndefined()
                    expect(r_close2.ok).toBe(true)
                    done()
                  })
                })
                expect(r_close1.error).toBeUndefined()
                expect(r_close1.ok).toBe(true)
              }
            })
          })
          expect(result1.error).toBeUndefined()
          expect(result1.ok).toBe(true)
        })
      }
    )

    expect(r_cr.error).toBeUndefined()
    expect(r_cr.ok).toBe(true)
  })

  test('getArchiveProperties', function (this: Context, done) {
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
          expect(reader.numberOfEntries).toBe(num_of_items)

          const result1 = reader.getArchiveProperties({}, (result2) => {
            process.nextTick(() => {
              console.dir(result2, { depth: null })

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
          expect(result1.error).toBeUndefined()
          expect(result1.ok).toBe(true)
        })
      }
    )

    expect(r_cr.error).toBeUndefined()
    expect(r_cr.ok).toBe(true)
  })
})
