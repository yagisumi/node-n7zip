type ErrorLike = {
  name: string
  message: string
}

function isErrorLike(obj: any): obj is ErrorLike {
  return (
    typeof obj === 'object' &&
    obj != null &&
    'name' in obj &&
    'message' in obj &&
    typeof obj.name === 'string' &&
    typeof obj.message === 'string'
  )
}

export function wrapError(err: unknown): Error {
  if (typeof err === 'string') {
    return new Error(err)
  } else if (err instanceof Error) {
    return err
  } else if (isErrorLike(err)) {
    const e = new Error(err.message)
    e.name = err.name
    return e
  } else {
    return new Error('unexpected error')
  }
}

export type OK<T> = {
  ok: true
  error: undefined
  value: T
}

export type ERR = {
  ok: false
  error: Error
  value: undefined
}

export function OK<T>(value: T): OK<T> {
  return {
    ok: true,
    error: undefined,
    value,
  }
}

export function ERR(error: unknown): ERR {
  return {
    ok: false,
    error: wrapError(error),
    value: undefined,
  }
}

export type Result<T> = OK<T> | ERR
