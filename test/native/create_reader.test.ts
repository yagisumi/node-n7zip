import { n7zip_native } from '@/n7zip_native'
import { n7zip } from '@/n7zip'
import path from 'path'

describe('create_reader', () => {
  describe.only('check error', () => {
    test('invalid arg', () => {
      if (!n7zip.DEBUG || n7zip_native.tester == null) {
        return
      }

      const r = n7zip_native.createReader2(null as any)
      console.log(r)
    })
  })
})
