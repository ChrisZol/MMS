//
//  main.cpp
//

#include "mms_image.hpp"
#include "mms_image_arithmetic.hpp"
#include "mms_image_histogram_tools.hpp"
#include "mms_image_io.hpp"
#include "mms_image_tools.hpp"
#include "mms_canny.hpp"
#include "mms_hough_transformation.hpp"
#include "mms_segmentation.hpp"
#include "mms_classification.hpp"

#include <string>
#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace mms;
using namespace cv;

int main(int argc, const char* argv[])
{
    Classification cls;
    cls.segmentation("../MMS");
    return 0;
}
