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

export type PropertyInfo = {
  name?: string
  propID: number
  varType: number
}

export type ReaderPropertyInfo = {
  archive: PropertyInfo[]
  entry: PropertyInfo[]
}

type Prop = {
  id: number
  value: number | string | boolean | undefined
}

export declare class Reader {
  getNumberOfItems(): number
  getNumberOfArchiveProperties(): number
  getNumberOfProperties(): number
  isClosed(): boolean
  close(cb: (r: Result<undefined>) => void): Result<undefined>
  getPropertyInfo(cb: (r: Result<ReaderPropertyInfo>) => void): Result<undefined>
  getArchiveProperties(
    opts: { propIDs?: number[] },
    cb: (r: Result<Array<Prop>>) => void
  ): Result<undefined>
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
    TRACE(msg: string): void
  }
}
