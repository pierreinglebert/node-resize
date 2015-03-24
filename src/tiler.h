#ifndef TILER_H
#define TILER_H

#include <vector>

struct Tile {
  int x;
  int y;
  int z;
  int width;
  int height;
  std::vector<unsigned char> data;
  Tile(int x, int y, int z, int width, int height): x(x), y(y), z(z), width(width), height(height), data(std::vector<unsigned char>(0)) { }
};

struct TiledImage {
  int width;
  int height;
  std::vector<Tile*> tiles;
  TiledImage(): width(0), height(0) {
    tiles = std::vector<Tile*>(0);
  };
  TiledImage(int width, int height): width(width), height(height) {
    tiles = std::vector<Tile*>(0);
  };
  ~TiledImage() {
    for(std::vector<Tile*>::const_iterator it = tiles.begin(); it != tiles.end(); it++) {
      delete *it;
    }
    tiles.clear();
  }
};

std::shared_ptr<TiledImage> compute(cv::Mat&, unsigned int, bool, unsigned int);

#endif
