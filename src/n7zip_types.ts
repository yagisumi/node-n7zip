export type OK<T = undefined> = {
  ok: true
  error: undefined
  value: T
}

export type ERR = {
  ok: false
  error: Error
  value: undefined
}

export type Result<T = undefined> = OK<T> | ERR

export type Format = {
  index: number
  name: string
  extension: string
  addExtension: string
  classId: string
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

declare class SharedLocker {
  run(cb: () => void): Result
}

export interface n7zipNativeType {
  DEBUG: boolean
  ARCH: 32 | 64
  loadLibrary(path: string): Result<boolean>
  getFormats(): Array<Format>
  getCodecs(): Array<Codec>
  SharedLocker?: typeof SharedLocker
}
