import path from 'path'
import { n7zip_native } from './n7zip_native'
import { Format, Codec } from './n7zip_native_types'

let formats: Readonly<Array<Readonly<Format>>> = []
let codecs: Readonly<Array<Readonly<Codec>>> = []

if (!('toNamespacedpath' in path)) {
  path.toNamespacedPath = function (filepath: string) {
    return filepath
  }
}

export function loadLibrary(libPath: string) {
  const path2 = path.toNamespacedPath(libPath)
  let r = n7zip_native.loadLibrary(path2)

  if (r.error) {
    const path3 = libPath.replace(/\\/g, '/')
    const asar = '/app.asar/'
    const idx = path3.lastIndexOf(asar)
    if (idx >= 0) {
      const path4 = path.toNamespacedPath(
        path3.slice(0, idx) + '/app.asar.unpacked/' + path3.slice(idx + asar.length)
      )
      r = n7zip_native.loadLibrary(path4)
    }
  }

  if (r.ok) {
    formats = n7zip_native.getFormats()
    formats.forEach((fmt) => Object.freeze(fmt))
    Object.freeze(formats)

    codecs = n7zip_native.getCodecs()
    codecs.forEach((codec) => Object.freeze(codec))
    Object.freeze(codecs)
  }

  return r
}

function getFormats() {
  return formats
}

function getCodecs() {
  return codecs
}

export const n7zip = {
  DEBUG: n7zip_native.DEBUG,
  ARCH: n7zip_native.ARCH,
  loadLibrary,
  getFormats,
  getCodecs,
}

function init() {
  let lib = ''
  if (process.platform === 'win32') {
    lib = path.resolve(__dirname, `../ext/files/7z${n7zip_native.ARCH}/7z.dll`)
  } else {
    lib = path.resolve(__dirname, `../build/${n7zip_native.DEBUG ? 'Debug' : 'Release'}/7z.so`)
  }
  const r = loadLibrary(lib)

  if (r.error) {
    throw r.error
  }

  if (process.platform !== 'win32') {
    const rar = path.resolve(
      __dirname,
      `../build/${n7zip_native.DEBUG ? 'Debug' : 'Release'}/Codecs/Rar.so`
    )
    loadLibrary(rar)
  }
}

init()
