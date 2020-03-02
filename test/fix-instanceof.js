'use strict'
const error = Error
const toString = Object.prototype.toString

const originalHasInstance = error[Symbol.hasInstance]

Object.defineProperty(error, Symbol.hasInstance, {
  value(potentialInstance) {
    return this === error
      ? toString.call(potentialInstance) === '[object Error]'
      : originalHasInstance.call(this, potentialInstance)
  },
})
// https://github.com/facebook/jest/issues/2549
