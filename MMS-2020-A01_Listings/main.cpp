//
//  main.cpp
//

#include "mms_image.hpp"
#include "mms_image_arithmetic.hpp"
#include "mms_image_io.hpp"
#include "mms_image_tools.hpp"

#include <string>
#include <iostream>

using namespace mms;

int main(int argc, const char * argv[])
{
    TImage<int> result_image(480, 615);
    vector<TImage<unsigned char>> images;

    for (int i = 0; i < 10; i++) {
        TImage<unsigned char> image;
        std::string filename = "../MMS-2020-A02c_Daten/applesNoise" + std::to_string(i) + ".pgm";
        if (readPGM(filename, image)) {
            images.push_back(image);
        }
        mms::addImageToImage(image, result_image);
    }

    clampImage(0, 255, result_image);
    divImageByScalar<int>(10, result_image);
    savePGM("../MMS-2020-A02c_Daten/applesNoise_result.pgm", result_image);

    return 0;
}

