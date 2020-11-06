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
const formats = all_formats.filter((fmt) => fmt.name === 'zip').map((fmt) => fmt.index)

function proxyCreateReader(arg: {
  streams: InStreamArg[]
  formats: number[]
  baseDir?: string | undefined
  password?: string | undefined
}): Promise<Result<Reader>> {
  return new Promise((resolve) => {
    const r1 = n7zip_native.createReader(arg, (r2) => {
      resolve(r2)
    })
    if (r1.error) {
      resolve(r1)
    }
  })
}

describe('archives', () => {
  test.only('zip', (done) => {
    const r_cr = n7zip_native.createReader({ baseDir, streams, formats }, (r_reader) => {
      console.log({ r_reader })
      expect(r_reader.error).toBeUndefined()
      done()
      // nextTick(() => {
      //   expect(r_reader.error).toBeUndefined()
      //   done()
      // })
    })

    console.log({ r_cr })

    expect(r_cr.error).toBeUndefined()
    expect(r_cr.ok).toBe(true)
  })
})
