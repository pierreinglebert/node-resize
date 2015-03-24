const tiler = require('../index');
const assert = require('chai').assert;
const fs = require('fs');
const path = require('path');
const async = require('async');

describe('Stream:', function() {

	it('should return tiles & infos for an image', function(done) {
		tiler(fs.createReadStream('test/fixtures/image.jpg'))
		.done(function(res) {
			assert.equal(res.width, 467);
			assert.equal(res.height, 700);
			assert.equal(res.tiles.length, 9);
			done();
		});
	});

	it('should fail to tile an empty image', function(done) {
		tiler(fs.createReadStream('test/fixtures/empty.jpg'))
		.catch(function(err){
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
