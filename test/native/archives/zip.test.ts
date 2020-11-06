import { n7zip_native, InStreamArg } from '@/n7zip_native'
import path from 'path'

const baseDir = path.resolve(__dirname, '../../files')
const name = 'archive.zip'
const streams: InStreamArg[] = [
  {
    type: 'path',
    source: path.join(baseDir, name),
    name,
  },
]

const all_formats = n7zip_native.getFormats()
const format_zip = all_formats.filter((fmt) => fmt.name === 'zip').map((fmt) => fmt.index)
const format_7zip = all_formats.filter((fmt) => fmt.name === '7z').map((fmt) => fmt.index)

describe('archives', () => {
  test('non-existent zip file', (done) => {
    const r_cr = n7zip_native.createReader(
      {
        streams: [{ type: 'path', source: 'non-existent.zip', name: '' }], //
        formats: format_zip,
      },
      (r_reader) => {
        process.nextTick(() => {
          expect(r_reader.error).toBeInstanceOf(Error)
          expect(r_reader.ok).toBe(false)
          expect(r_reader.error?.message).toBe('Cannot open file: non-existent.zip')
          done()
        })
      }
    )

    expect(r_cr.error).toBeUndefined()
    expect(r_cr.ok).toBe(true)
  })

  test('incorrect format', (done) => {
    const r_cr = n7zip_native.createReader(
      {
        streams, //
        formats: format_7zip,
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

  test('open', (done) => {
    const r_cr = n7zip_native.createReader(
      {
        streams, //
        formats: format_zip,
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
          expect(reader.getNumberOfItems()).toBe(29)

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
})
