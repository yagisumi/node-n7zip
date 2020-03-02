import { n7zipNativeType } from './n7zip_native'
const n7zipNative = require('bindings')('n7zip') as n7zipNativeType

export const n7zip = {
  DEBUG: n7zipNative.DEBUG,
  ARCH: n7zipNative.ARCH,
}
