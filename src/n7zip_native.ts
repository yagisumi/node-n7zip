import { n7zipNativeType } from './n7zip_native_types'
// eslint-disable-next-line @typescript-eslint/no-var-requires
export const n7zip_native = require('bindings')('n7zip') as n7zipNativeType
