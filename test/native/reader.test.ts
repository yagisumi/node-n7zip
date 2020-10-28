import { n7zip_native, InStreamArg } from '@/n7zip_native'
import path from 'path'

const file = path.resolve(__dirname, '../files/archive.zip')

describe('Reader', () => {
  test.only('createReader', () => {
    const formats = n7zip_native.getFormats()
    console.log(formats.filter((fmt) => ['zip', '7z', '7zip'].includes(fmt.name)))
    // console.log(formats)
    const fmtIndices = formats
      // .filter((fmt) => ['zip', '7z'].includes(fmt.name))
      .filter((fmt) => ['7z'].includes(fmt.name))
      .map((fmt) => fmt.index)
    const files: InStreamArg[] = [
      // { name: 'archive_rar.zip', source: path.resolve(__dirname, '../files/archive_rar.zip') },
      {
        name: 'archive.7z',
        source: [
          { source: path.resolve(__dirname, '../files/archive.7z.001') },
          { source: path.resolve(__dirname, '../files/archive.7z.002') },
        ],
      },
    ]
    // const files = [['in_stream.txt', file]]
    console.log(
      n7zip_native.createReader({
        formats: fmtIndices,
        streams: files,
        // baseDir: path.dirname(path.resolve(__dirname, '../files/archive_rar.zip')) + path.sep,
      })
    )
  })
})
