import { Result, Format, Codec } from './n7zip_types'
// eslint-disable-next-line @typescript-eslint/no-var-requires
export const n7zip_native = require('bindings')('n7zip') as n7zipNativeType

declare class SharedLocker {
  run(cb: () => void): Result
}

export interface n7zipNativeType {
  DEBUG: boolean
  ARCH: 32 | 64
  loadLibrary(path: string): Result<boolean>
  getFormats(): Array<Format>
  getCodecs(): Array<Codec>

  tester?: {
    SharedLocker: typeof SharedLocker
  }
}
