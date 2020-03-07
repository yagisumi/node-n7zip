export type OK<T = undefined> = {
  ok: true
  error: undefined
  value: T
}

export type ERR = {
  ok: false
  error: Error
}

export type Result<T = undefined> = OK<T> | ERR
