'use strict';

var resize = require('..');
var assert = require('chai').assert;
var fs = require('fs');
var path = require('path');
var async = require('async');

describe('resize:', function() {

	it.only('should resize an image', function(done) {
		resize(
			path.join(__dirname, '..', 'test/fixtures/image.jpg'),
			path.join(__dirname, '..', 'test.jpg'),
			{
				width: 50,
				height: 50
			},
			function(err) {
				assert.isNull(err);
				done();
			}
		);
	});

	it('should fail to tile an empty image', function(done) {
		tiler(fs.createReadStream('test/fixtures/empty.jpg'))
		.catch(function(err) {
			assert.isNotNull(err);
		})
		.done(function(res) {
			assert.isUndefined(res);
			done();
		});
	});

	it('should not explode with a broken image', function(done) {
		tiler(fs.createReadStream('test/fixtures/broken.jpg'))
		.catch(function(err){
			assert.isNotNull(err);
		})
		.done(function(res) {
			assert.isUndefined(res);
			done();
		});
	});

	it('should tile multiple images', function(done) {
		this.timeout(5000);
		var images = [
			'1M.jpg',
			'1M.jpg',
			'1M.jpg',
			'1M.jpg',
			'1M.jpg'
		];
		async.each(images,
		function(image, next) {
			tiler(fs.createReadStream(path.join('test/fixtures/', image)))
			.catch(function(err){
				assert.isNull(err);
			})
			.done(function(res) {
				assert.isNotNull(res);
				next();
			});
		},
		function(err) {
			done(err);
		});
	});

});
