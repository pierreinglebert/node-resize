'use strict';

var path = require('path');
var resizer = require('./');

var input = path.join(__dirname, 'test/fixtures/1M.jpg');
var output = path.join(__dirname, 'resized.jpg');

console.log(input);

resizer(input, output, function(err, res) {
	console.log(err, res);
});
