#include <node.h>
#include <node_buffer.h>
#include <vector>
#include <string>

// OpenCV libs
#include <cv.h>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/ocl/ocl.hpp"

#include "nan.h"

#include "tiler.h"

#define DEBUG 0
#define debug_print(fmt, ...) do { if (DEBUG) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, __VA_ARGS__); } while (0)

using namespace v8;
using namespace node;
using namespace cv;

class ResizeWorker : public NanAsyncWorker {
 public:
   explicit ResizeWorker(std::string input, std::string output, unsigned int width, unsigned int height, bool sharpen, unsigned int quality, NanCallback *callback)
    : NanAsyncWorker(callback), input(input), output(output), width(width), height(height), sharpen(sharpen), quality(quality) {
      printf("constructor\n");
    }
  ~ResizeWorker() { }

  void Execute () {

    try {
      Mat image = cv::imread(input, CV_LOAD_IMAGE_COLOR);

      if (image.empty()){
        printf("EMPTY %s\n", input.c_str() );
        SetErrorMessage("Error loading file");
        return;
      }

      if(image.cols == width && image.rows == height) {
        imwrite(output, image);
    	} else {
        Mat resized(width, height, CV_8UC1);
    		cv::resize(image, resized, resized.size(), 0, 0);
    		if(sharpen) {
    			cv::GaussianBlur(resized, resized, cv::Size(0, 0), 3);
    			cv::addWeighted(resized, 1.5, resized, -0.5, 0, resized);
    		}
        imwrite(output, resized);
    	}


    } catch(cv::Exception& e) {
      printf("EXCEPTION\n");
      SetErrorMessage("Error loading file");
    }
  }

  void HandleOKCallback () {
    NanScope();
    // Local<Array> tiles = NanNew<Array>(tiledImage->tiles.size());
    //
    // for(unsigned int i=0; i < tiledImage->tiles.size(); i++) {
    //   Local<Object> tile = NanNew<Object>();
    //   tile->Set(NanNew<v8::String>("x"), NanNew<Number>(tiledImage->tiles[i]->x));
    //   tile->Set(NanNew<v8::String>("y"), NanNew<Number>(tiledImage->tiles[i]->y));
    //   tile->Set(NanNew<v8::String>("z"), NanNew<Number>(tiledImage->tiles[i]->z));
    //   tile->Set(NanNew<v8::String>("width"), NanNew<Number>(tiledImage->tiles[i]->width));
    //   tile->Set(NanNew<v8::String>("height"), NanNew<Number>(tiledImage->tiles[i]->height));
    //
    //   Local<Object> buf = NanNewBufferHandle(tiledImage->tiles[i]->data.size());
    //   uchar* data = (uchar*) Buffer::Data(buf);
    //   memcpy(data, &(tiledImage->tiles[i]->data)[0], tiledImage->tiles[i]->data.size());
    //
    //   tile->Set(NanNew<String>("buffer"), buf);
    //   tiles->Set(i, tile);
    // }

    Local<Object> image = NanNew<Object>();
    // image->Set(NanNew<String>("width"), NanNew<Number>(tiledImage->width));
    // image->Set(NanNew<String>("height"), NanNew<Number>(tiledImage->height));
    // image->Set(NanNew<String>("tiles"), tiles);

    Local<Value> argv[] = {
        NanNull()
      , image
    };
    callback->Call(2, argv);
  }

 private:
  std::string input;
  std::string output;
  int width;
  int height;
  bool sharpen;
  int quality;
};

NAN_METHOD(ResizeAsync) {
  NanScope();

  NanCallback *callback = new NanCallback(args[6].As<Function>());

  TryCatch try_catch;
  printf("start\n");

  // std::string strStrategy(*NanUtf8String(filter_strategies->Get(i)->ToString()));


  NanAsyncQueueWorker(new ResizeWorker(*NanUtf8String(args[0]), *NanUtf8String(args[1]), args[2]->Uint32Value(), args[3]->Uint32Value(), args[4]->BooleanValue(), args[5]->Uint32Value(), callback));

  if (try_catch.HasCaught()) {
    FatalException(try_catch);
  }

  NanReturnUndefined();
}

static void at_exit(void* arg) {
  NanScope();
}

extern "C" void RegisterModule(Handle<Object> target) {
  NanScope();
  AtExit(at_exit);
  target->Set(NanNew<v8::String>("resize"), NanNew<FunctionTemplate>(ResizeAsync)->GetFunction());
}

NODE_MODULE(resizer, RegisterModule);
