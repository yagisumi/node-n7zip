import { n7zip } from '@/n7zip'
import { TRACE } from '?/utils'

describe('n7zip_native', function () {
  test('native module properties', function (this: Context) {
    TRACE(this)

    expect(n7zip).toHaveProperty('DEBUG')
    expect(typeof n7zip.DEBUG).toBe('boolean')

    expect(n7zip).toHaveProperty('ARCH')
    expect(n7zip.ARCH).toBe(['ia32', 'x32'].includes(process.arch) ? 32 : 64)
  })
})
