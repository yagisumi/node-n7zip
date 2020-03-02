const debug = (process.argv[2] || '').indexOf('Debug') >= 0

if (!debug) {
  const fs = require('fs')
  const path = require('path')
  const dir = path.resolve(__dirname, '../7z')
  if (fs.existsSync(dir)) {
    const rimraf = require('rimraf')
    try {
      rimraf.sync(dir)
    } catch (e) {}
  }
}
