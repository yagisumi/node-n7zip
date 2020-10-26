import { Result, Format, Codec } from './n7zip_types'
// eslint-disable-next-line @typescript-eslint/no-var-requires
export const n7zip_native = require('bindings')('n7zip') as n7zipNativeType

declare class SharedLocker {
  run(cb: () => void): Result
}

export enum SeekOrigin {
  SEEK_SET,
  SEEK_CUR,
  SEEK_END,
}

export declare class InStreamWrap {
  read(size: number): Result<Buffer>
  seek(offset: number, seekOrigin: SeekOrigin): Result<number>
}

export type InStreamData = [string, string] | [string, number, boolean?]

export type FdInStreamArg = {
  source: number
  AutoClose?: boolean
}

export type FileInStreamArg = {
  source: string
}

export type BufferInStreamArg = {
  source: Buffer
  ShareBuffer?: boolean
}

export type InStreamSubArg = FdInStreamArg | FileInStreamArg | BufferInStreamArg

export type MultiInStreamArg = {
  source: Array<InStreamSubArg>
}

export type WithName = { name: string }
export type InStreamArg = (InStreamSubArg | MultiInStreamArg) & WithName

export interface n7zipNativeType {
  DEBUG: boolean
  ARCH: 32 | 64
  loadLibrary(path: string): Result<boolean>
  getFormats(): Array<Format>
  getCodecs(): Array<Codec>
  createReader(streams: InStreamData[], fmtIndices: number[]): Result<undefined>

  tester?: {
    SharedLocker: typeof SharedLocker
    createInStream(stream: InStreamArg): Result<InStreamWrap>
    inspectUString(str: string): Buffer | undefined
    convertUStringToAString(str: string): Buffer | undefined
  }
}
