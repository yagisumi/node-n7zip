import { n7zip_native } from '@/n7zip_native'

describe('utils', () => {
  test('inspectUString', () => {
    if (!n7zip_native.DEBUG || n7zip_native.tester == null) {
      return
    }

    const r = n7zip_native.tester.inspectUString('𠮷')
    console.log(r)
  })
})
