#include <cv.h>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/ocl/ocl.hpp"

#include "tiler.h"

using namespace cv;
using namespace std;

std::shared_ptr<TiledImage> compute(Mat& image, unsigned int tileSize, bool sharpen, unsigned int quality) {

  double imageWidth = (double)image.rows;
  double imageHeight = (double)image.cols;

  //todo check 0 sized images
  int maxLevel = std::max(0., ceil(log2(std::max(imageWidth, imageHeight) / tileSize)));

  std::shared_ptr<TiledImage> tiledImage(new TiledImage(imageWidth, imageHeight));

  // TODO parallel_for
  // C'est du multithread donc c'est pas si simple
  for(int level = 0; level <= maxLevel; level++) {
    unsigned int thumbnailWidth = floor(imageWidth / pow(2, level));
    unsigned int thumbnailHeight = floor(imageHeight / pow(2, level));

    Mat thumbnail;

    // Resize image if needed
    if(level == 0) {
      // TODO Do we really need a copy here ?
    	thumbnail = image;
    } else {
    	thumbnail = Mat(thumbnailWidth, thumbnailHeight, CV_8UC1);
  		resize(image, thumbnail, thumbnail.size(), 0, 0);
  		if(sharpen) {
  			GaussianBlur(thumbnail, thumbnail, Size(0, 0), 3);
  			addWeighted(thumbnail, 1.5, thumbnail, -0.5, 0, thumbnail);
  		}
    }

    // Now we cut resized image
    int columns = ceil((double)thumbnailWidth / tileSize);
    int rows = ceil((double)thumbnailHeight / tileSize);
    for(int i=0; i<columns; i++) {
      for(int j=0; j<rows; j++) {
        // on copie la zone
        int z = maxLevel - level;
    		int x = i * tileSize;
    		int y = j * tileSize;
    		int sizeX = min(tileSize, thumbnailWidth - x);
    		int sizeY = min(tileSize, thumbnailHeight - y);
    		Mat subImage(thumbnail, cv::Rect(y, x, sizeY, sizeX));

    		Tile* tile = new Tile(i, j, z, subImage.cols, subImage.rows);

    		std::vector<int> params;
    		params.push_back(CV_IMWRITE_JPEG_QUALITY);
    		params.push_back(quality);

        cv::imencode(".jpg", subImage, tile->data, params);
        subImage.release();
    		tiledImage->tiles.push_back(tile);
      }
    }
    thumbnail.release();
  }
  return tiledImage;
}

/*
OpenCL version
ocl::DevicesInfo devices;
ocl::getOpenCLDevices(devices, ocl::CVCL_DEVICE_TYPE_GPU);
for(int i = 0 ; i < devices.size() ; ++i)
{
  std::cout << "Device : " << devices[i]->deviceName << " is present" << std::endl;
}
cv::ocl::setDevice(devices[0]);
ocl::oclMat imageCL(image);
ocl::oclMat smallImg(800, 600, CV_8UC1);

ocl::resize(imageCL, smallImg, smallImg.size(), 0, 0);
ocl::GaussianBlur(smallImg, smallImg, Size(0, 0), 3);
ocl::addWeighted(smallImg, 1.5, smallImg, -0.5, 0, smallImg);
std::vector<uchar> vec(0);
imencode("jpg", smallImg, vec);
*/
