import { n7zip } from '@/n7zip'
import { n7zip_native } from '@/n7zip_native'
import path from 'path'

describe('library', () => {
  test('original formats and codecs', () => {
    const formats = n7zip
      .getFormats()
      .map((fmt) => fmt.name)
      .sort()

    const defaultFormats = [
      '7z',
      'APM',
      'Ar',
      'Arj',
      'Cab',
      'Chm',
      'Compound',
      'Cpio',
      'CramFS',
      'Dmg',
      'ELF',
      'Ext',
      'FAT',
      'FLV',
      'GPT',
      'HFS',
      'Hxs',
      'IHex',
      'Iso',
      'Lzh',
      'MBR',
      'MachO',
      'MsLZ',
      'Mub',
      'NTFS',
      'Nsis',
      'PE',
      'Ppmd',
      'QCOW',
      'Rar',
      'Rar5',
      'Rpm',
      'SWF',
      'SWFc',
      'Split',
      'SquashFS',
      'TE',
      'UEFIc',
      'UEFIf',
      'Udf',
      'VDI',
      'VHD',
      'VMDK',
      'Xar',
      'Z',
      'bzip2',
      'gzip',
      'lzma',
      'lzma86',
      'tar',
      'wim',
      'xz',
      'zip',
    ]
    if (process.platform === 'win32') {
      defaultFormats.push('COFF') // since 7zip 18.00
      defaultFormats.sort()
    }
    expect(formats).toEqual(defaultFormats)

    const codecs = n7zip
      .getCodecs()
      .map((codec) => codec.name)
      .sort()
    const defaultCodecs = [
      '7zAES',
      'AES256CBC',
      'ARM',
      'ARMT',
      'BCJ',
      'BCJ2',
      'BZip2',
      'Copy',
      'Deflate',
      'Deflate64',
      'Delta',
      'IA64',
      'LZMA',
      'LZMA2',
      'PPC',
      'PPMD',
      'Rar1',
      'Rar2',
      'Rar3',
      'Rar5',
      'SPARC',
      'Swap2',
      'Swap4',
    ]
    expect(codecs).toEqual(defaultCodecs)

    // console.log({ length: n7zip.getFormats().length })
    // console.log(n7zip.getFormats())
    // console.log(n7zip.getFormats().filter((fmt) => fmt.canUpdate))
    // console.log(n7zip.getCodecs())
  })

  test('loadLibrary should fail when it fails to acquire a lock', (done) => {
    if (!n7zip.DEBUG) {
      done()
      return
    }

    const lib =
      process.platform === 'win32'
        ? path.resolve(__dirname, `../ext/files/7z${n7zip.ARCH}/7z.dll`)
        : path.resolve(__dirname, `../build/${n7zip.DEBUG ? 'Debug' : 'Release'}/7z.so`)

    if (n7zip_native.tester != null) {
      let e: any = undefined
      setTimeout(() => {
        if (e != null) {
          throw e
        } else {
          done()
        }
      }, 3000)

      try {
        const r1 = n7zip.loadLibrary(lib)
        expect(r1.error).toBeUndefined()
        expect(r1.ok).toBe(true)

        const locker = new n7zip_native.tester.SharedLocker()
        const r2 = locker.run(() => {})
        expect(r2.error).toBeUndefined()
        expect(r2.ok).toBe(true)

        // SharedLocker can't run at the same time.
        const r3 = locker.run(() => {})
        expect(r3.error).toBeInstanceOf(Error)
        expect(r3.ok).toBe(false)

        const r4 = n7zip.loadLibrary(lib)
        expect(r4.error).toBeInstanceOf(Error)
        expect(r4.ok).toBe(false)
      } catch (err) {
        e = err
      }
    } else {
      done()
    }
  })
})
