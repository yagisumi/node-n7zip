import { n7zip_native, InStreamArg, Reader } from '@/n7zip_native'
import { Result } from '@/n7zip_types'
import path from 'path'
import { nextTick } from 'process'

const baseDir = path.resolve('../../files')
const name = 'archive.zip'
const streams: InStreamArg[] = [
  {
    type: 'path',
    source: path.resolve(baseDir, name),
    name,
  },
]

const all_formats = n7zip_native.getFormats()
const format_zip = all_formats.filter((fmt) => fmt.name === 'zip').map((fmt) => fmt.index)
const format_7zip = all_formats.filter((fmt) => fmt.name === 'zip').map((fmt) => fmt.index)

describe('archives', () => {
  test.only('non-existent zip', (done) => {
    const r_cr = n7zip_native.createReader(
      {
        streams: [{ type: 'path', source: 'non-existent.zip', name: '' }], //
        formats: format_zip,
      },
      (r_reader) => {
        console.log({ r_reader })
        expect(r_reader.error).toBeUndefined()
        done()
        // nextTick(() => {
        //   expect(r_reader.error).toBeUndefined()
        //   done()
        // })
      }
    )

    console.log({ r_cr })

    expect(r_cr.error).toBeUndefined()
    expect(r_cr.ok).toBe(true)
  })
})
