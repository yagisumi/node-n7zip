import { n7zip_native } from '@/n7zip_native'
import path from 'path'

const file = path.resolve(__dirname, '../files/in_stream.txt')

describe('Reader', () => {
  test.only('createReader', () => {
    const formats = n7zip_native.getFormats()
    const fmtIndices = formats.filter((fmt) => ['zip'].includes(fmt.name)).map((fmt) => fmt.index)
    console.log(n7zip_native.createReader([['in_stream.txt', file]], fmtIndices))
  })
})
