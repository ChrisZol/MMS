//
//  main.cpp
//

#include "mms_image.hpp"
#include "mms_image_arithmetic.hpp"
#include "mms_image_histogram_tools.hpp"
#include "mms_image_io.hpp"
#include "mms_image_tools.hpp"
#include "mms_canny.hpp"

#include <string>
#include <iostream>

using namespace mms;

int main(int argc, const char * argv[])
{
    TImage<unsigned char> image;

    std::string filename = "../MMS-2020-A03_Bild";
    if (readPGM(filename + ".pgm", image)) {
        TImage<unsigned char> gauss_img;
        TImage<int> grad_sum_img(image.getWidth(), image.getHeight());
        TImage<int> grad_dir_img(image.getWidth(), image.getHeight());
        TImage<int> nonMaxima_img;
        TImage<int> hysterese_img;

        gauss_img = gauss_filter(image, 1.2f);
        gradient(image, grad_sum_img, grad_dir_img);
        nonMaxima_img = nonMaxima_suppression<int, int>(grad_sum_img, grad_dir_img);
        hysterese_img = hysterese<int, int>(nonMaxima_img, 60.0f, 40.0f);

        //Saving all the images for presentation purposes
        savePGM(filename + "_gauss.pgm", gauss_img);
        savePGM(filename + "_grad_sum.pgm", grad_sum_img);
        savePGM(filename + "_grad_dir.pgm", grad_dir_img);
        savePGM(filename + "_nonMaxima.pgm", nonMaxima_img);
        savePGM(filename + "_hysterese.pgm", hysterese_img);
    }

    return 0;
}

