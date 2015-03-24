#include <node.h>
#include <node_buffer.h>
#include <vector>
#include <string>

// OpenCV libs
#include <cv.h>
#include <opencv2/highgui/highgui.hpp>
//#include "opencv2/ocl/ocl.hpp"

#include "nan.h"

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

    Local<Value> argv[] = {
     NanNull()
    };
    callback->Call(1, argv);
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
