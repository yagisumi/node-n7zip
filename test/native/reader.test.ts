import { n7zip_native, InStreamData } from '@/n7zip_native'
import path from 'path'

const file = path.resolve(__dirname, '../files/archive.zip')

describe('Reader', () => {
  test('createReader', () => {
    const formats = n7zip_native.getFormats()
    console.log(formats.filter((fmt) => ['zip', '7z', '7zip'].includes(fmt.name)))
    // console.log(formats)
    const fmtIndices = formats
      // .filter((fmt) => ['zip', '7z'].includes(fmt.name))
      .filter((fmt) => ['zip'].includes(fmt.name))
      .map((fmt) => fmt.index)
    const files: InStreamData[] = [
      // ['archive.7z.001', path.resolve(__dirname, '../files/archive.7z.001')],
      // ['archive.7z.002', path.resolve(__dirname, '../files/archive.7z.002')],
      ['archive_rar.z01', path.resolve(__dirname, '../files/archive_rar.z01')],
      ['archive_rar.z02', path.resolve(__dirname, '../files/archive_rar.z02')],
      ['archive_rar.zip', path.resolve(__dirname, '../files/archive_rar.zip')],
    ]
    // const files = [['in_stream.txt', file]]
    console.log(n7zip_native.createReader(files, fmtIndices))
  })
})
