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
  type: 'fd'
  source: number
  AutoClose?: boolean
}

export type FileInStreamArg = {
  type: 'path'
  source: string
}

export type BufferInStreamArg = {
  type: 'buffer'
  source: Buffer
  ShareBuffer?: boolean
}

export type InStreamSubArg = FdInStreamArg | FileInStreamArg | BufferInStreamArg

export type MultiInStreamArg = {
  type: 'multi'
  source: Array<InStreamSubArg>
}

export type WithName = { name: string }
export type InStreamArg = (InStreamSubArg | MultiInStreamArg) & WithName

declare class Reader {
  getNumberOfItems(): number
  getNumberOfArchiveProperties(): number
  getNumberOfProperties(): number
  close(cb: (r: Result) => void): Result
  isClosed(): boolean
}

export interface n7zipNativeType {
  DEBUG: boolean
  ARCH: 32 | 64
  loadLibrary(path: string): Result<boolean>
  getFormats(): Array<Format>
  getCodecs(): Array<Codec>
  createReader(
    arg: {
      streams: InStreamArg[]
      formats: number[]
      baseDir?: string
      password?: string
    },
    cb: (r: Result<Reader>) => void
  ): Result<undefined>

  tester?: {
    SharedLocker: typeof SharedLocker
    createInStream(stream: InStreamArg): Result<InStreamWrap>
    inspectUString(str: string): Buffer | undefined
    convertUStringToAString(str: string): Buffer | undefined
    isObject(x: unknown): boolean
  }
}
