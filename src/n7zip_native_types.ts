import { Result } from 'result'

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

export declare class SharedLocker {
  run(cb: () => void): Result<undefined>
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

export type Prop = {
  id: number
  value: number | string | boolean | undefined
}

export type GetArchivePropertiesOptions = {
  propIDs?: number[]
}

export type GetEntriesOptions = {
  limit?: number
  start?: number
  end?: number
  propIDs?: number[]
  canceler?: Canceler
}

export type GetEntriesResponse =
  | {
      type: 'entries'
      done: boolean
      entries: { index: number; props: Prop[] }
    }
  | {
      type: 'abort'
    }
  | {
      type: 'error'
    }

export declare class Canceler {
  constructor(taskName?: string)
  get taskName(): string
  cancel(): void
}

export declare class Controller {
  constructor(taskName?: string)
  get taskName(): string
  cancel(): void
  _pause(): void
  _resume(): void
}

export type ExtractOptions = {
  index: number
  testMode?: boolean
  canceler?: Controller | Canceler
}

export type ExtractBufferResponse = {
  type: 'buffer'
  buffer: Buffer
  index: number
  offset: number
  done: boolean
  operationResult: number
}

export type ExtractErrorResponse = {
  type: 'error'
  message: ''
}

export type ExtractEndResponse =
  | {
      type: 'end'
      skipExtraction: true
      isDir: boolean
      isLink: boolean
    }
  | {
      type: 'end'
      skipExtraction: false
      extractResult: number
    }

export type ExtractResponse = ExtractBufferResponse | ExtractErrorResponse | ExtractEndResponse

export declare class Reader {
  get formatIndex(): number
  get formatName(): string
  get numberOfEntries(): number
  get numberOfArchiveProperties(): number
  get numberOfProperties(): number
  isClosed(): boolean
  close(cb: (r: Result<undefined>) => void): Result<undefined>
  getPropertyInfo(cb: (r: Result<ReaderPropertyInfo>) => void): Result<undefined>
  getArchiveProperties(
    opts: GetArchivePropertiesOptions,
    cb: (r: Result<Array<Prop>>) => void
  ): Result<undefined>
  getEntries(opts: GetEntriesOptions, cb: (res: GetEntriesResponse) => void): Result<undefined>
  extract(opts: ExtractOptions, cb: (r: ExtractEndResponse) => void): Result<undefined>
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
  Canceler: typeof Canceler
  Controller: typeof Controller

  tester?: {
    SharedLocker: typeof SharedLocker
    createInStream(stream: InStreamArg): Result<InStreamWrap>
    inspectUString(str: string): Buffer | undefined
    convertUStringToAString(str: string): Buffer | undefined
    isObject(x: unknown): boolean
    TRACE(msg: string): void
  }
}
