import { Result } from './n7zip_types'

export type Format = {
  name: string
  index: number
  canUpdate: boolean
  flags: {
    KeepName: boolean
    FindSignature: boolean
    AltStreams: boolean
    NtSecure: boolean
    SymLinks: boolean
    HardLinks: boolean
    UseGlobalOffset: boolean
    StartOpen: boolean
    BackwardOpen: boolean
    PreArc: boolean
    PureStartOpen: boolean
  }
}

export type Codec = {
  name: string
  index: number
  id: number
}

export interface n7zipNativeType {
  DEBUG: boolean
  ARCH: 32 | 64
  loadLibrary(path: string): Result<boolean>
  getFormats(): Array<Format>
  getCodecs(): Array<Codec>
}
