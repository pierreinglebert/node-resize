'use strict';

var resizer = require('bindings')('resizer.node');

module.exports = function tile(input, output, options, callback) {
  if(!callback && typeof options === 'function') {
    callback = options;
    options = {};
  }
  options = options || {};
  options.width = options.width || 1;
  options.height = options.height || 1;
  options.quality = options.quality || 80;
  options.sharpen = options.sharpen || false;
  resizer.resize(input, output, options.width, options.height, options.sharpen, options.quality, callback);
};
