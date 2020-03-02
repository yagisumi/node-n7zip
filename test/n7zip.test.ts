import { n7zip } from '@/n7zip'

describe('n7zip', () => {
  test('Native module properties', () => {
    expect(n7zip).toHaveProperty('DEBUG')
    expect(typeof n7zip.DEBUG).toBe('boolean')

    expect(n7zip).toHaveProperty('ARCH')
    expect(n7zip.ARCH).toBe(process.arch === 'x64' ? 64 : 32)
  })
})
