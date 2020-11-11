import { n7zip_native } from '@/n7zip_native'

function isContext(arg: any): arg is Context {
  return (
    'test' in arg &&
    'title' in arg.test &&
    typeof arg.test.fullTitle === 'function' &&
    typeof arg.test.titlePath === 'function'
  )
}

export function TRACE(ctx: any) {
  if (isContext(ctx)) {
    n7zip_native.tester?.TRACE('@test/' + ctx.test?.titlePath().join('/'))
  }
}
