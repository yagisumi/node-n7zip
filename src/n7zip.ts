import { toNamespacedPath, resolve } from 'path'
import { n7zipNativeType, Format, Codec } from './n7zip_native'
const n7zipNative = require('bindings')('n7zip') as n7zipNativeType

let formats: Array<Format> = []
let codecs: Array<Codec> = []

namespace Silent {
  export function loadLibrary(path: string) {
    const path2 = toNamespacedPath(path)
    let r = n7zipNative.loadLibrary(path2)

    if (r.error) {
      const path3 = path.replace(/\\/g, '/')
      const asar = '/app.asar/'
      const idx = path3.lastIndexOf(asar)
      if (idx >= 0) {
        const path4 = toNamespacedPath(path3.slice(0, idx) + '/app.asar.unpacked/' + path3.slice(idx + asar.length))
        r = n7zipNative.loadLibrary(path4)
      }
    }

    if (r.ok) {
      formats = n7zipNative.getFormats()
      formats.forEach((fmt) => Object.freeze(fmt))
      Object.freeze(formats)

      codecs = n7zipNative.getCodecs()
      codecs.forEach((codec) => Object.freeze(codec))
      Object.freeze(codecs)
    }

    return r
  }
}

function getFormats() {
  return formats
}

function getCodecs() {
  return codecs
}
export const n7zipSilent = {
  DEBUG: n7zipNative.DEBUG,
  ARCH: n7zipNative.ARCH,
  loadLibrary: Silent.loadLibrary,
  getFormats,
  getCodecs,
}

function loadLibrary(path: string) {
  const r = Silent.loadLibrary(path)
  if (r.ok) {
    return r.value
  } else {
    throw r.error
  }
}

export const n7zip = {
  DEBUG: n7zipNative.DEBUG,
  ARCH: n7zipNative.ARCH,
  loadLibrary,
  getFormats,
  getCodecs,
}

function init() {
  const dir = __dirname
  let lib = ''
  if (process.platform === 'win32') {
    lib = resolve(dir, `../ext/files/7z${n7zipNative.ARCH}/7z.dll`)
  } else {
    lib = resolve(dir, `../build/${n7zipNative.DEBUG ? 'Debug' : 'Release'}/7z.so`)
  }
  const r = Silent.loadLibrary(lib)

  if (r.error) {
    throw r.error
  }

  if (process.platform !== 'win32') {
    const rar = resolve(dir, `../build/${n7zipNative.DEBUG ? 'Debug' : 'Release'}/Codecs/Rar.so`)
    Silent.loadLibrary(rar)
  }
}
init()
