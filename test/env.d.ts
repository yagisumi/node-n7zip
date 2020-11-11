declare class Runnable {
  title: string
  file?: string
  fullTitle(): string
  titlePath(): string[]
}

declare class Context {
  test?: Runnable
}
