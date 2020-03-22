# cvUniText

A C++ library of drawing unicode text with user-defined font based on OpenCV 2.
Modified based on [this opencv 1 library](https://github.com/buaabyl/cvPutUniText)

Notice: Current implementations can't handle character spacing very well, so it's suggested to use a mono-spaced font to alleviate the problem. A mono-spaced font supporting Chinese is included in the `install/` directory. Will be fixed in the future.

## Compilation

This repo contains the source of the library and a pre-compiled version (with GCC 4.8.5, on our GPU servers). You can always try the pre-compiled version first, under `install/`. If it fails, compile as follows:

1. Navigate to the root of this repo. Run `mkdir build; cd build; cmake ..`
2. `make`
3. Copy `cvUniText.hpp` and `libcvunitext.so` to your project. Compile your project linking the library and opencv

## Example
```
    cv::Mat img = cv::imread("1.jpg");
    // Create an instance with font file path and size
    uni_text::UniText uniText("/usr/share/fonts/wqy-microhei/wqy-microhei.ttc", 80);
    // Specify the position and color of your text
    cv::Point p(100, 100);
    cv::Scalar color(0,255,0);
    // Draw the text. The return value is the bounding box of the text
    // Set the last argument to `true` in order to get the rect without actually drawing the text
    cv::Rect rect = uniText.PutText(img, "Hello, 哈哈哈哈嗝", p, color, false);
    std::cout << rect << std::endl;
    cv::imwrite("1.jpg", img);
    return 0;
```

Detailed comments can be found in `cvUniText.hpp`
