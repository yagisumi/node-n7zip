import { n7zip } from '@/n7zip'

describe('n7zip', () => {
  test('Native module properties', () => {
    expect(n7zip).toHaveProperty('DEBUG')
    expect(typeof n7zip.DEBUG).toBe('boolean')

    expect(n7zip).toHaveProperty('ARCH')
    expect(n7zip.ARCH).toBe(['ia32', 'x32'].includes(process.arch) ? 32 : 64)
  })

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

    // console.log(n7zip.getFormats())
    // console.log(n7zip.getCodecs())
  })
})
