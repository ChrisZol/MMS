//
//  main.cpp
//

#include "mms_image.hpp"
#include "mms_image_arithmetic.hpp"
#include "mms_image_histogram_tools.hpp"
#include "mms_image_io.hpp"
#include "mms_image_tools.hpp"

#include <string>
#include <iostream>

using namespace mms;

int main(int argc, const char * argv[])
{
    TImage<unsigned char> image;
    TImage<unsigned char> image2;

    std::string filename = "../MMS-2020-A03_Bild.pgm";
    if (readPGM(filename, image)) {
        histogram_stretching<unsigned char, int>(image, 0.05f, 0.95f);
        savePGM("../MMS-2020-A03_Bild_stretched.pgm", image);
    }

    if (readPGM(filename, image2)) {
        histogram_equalization<unsigned char, float>(image2);
        savePGM("../MMS-2020-A03_Bild_equalized.pgm", image2);
    }
    
    return 0;
}

