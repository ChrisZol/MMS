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

#include <string>
#include <iostream>

using namespace mms;

const int ONE_CENT_R = 45;
const int TWO_CENT_R = 50;
const int FIVE_CENT_R = 60;

int main(int argc, const char * argv[])
{
    TImage<unsigned char> image;

    std::string filename = "../MMS-2020-A05";
    if (readPGM(filename + ".pgm", image)) {
        TImage<unsigned char> gauss_img;
        TImage<int> grad_sum_img(image.getWidth(), image.getHeight());
        TImage<int> grad_dir_img(image.getWidth(), image.getHeight());
        TImage<int> nonMaxima_img;
        TImage<int> hysterese_img;
        TImage<int> hough_img0;
        TImage<int> hough_img1;
        TImage<int> hough_img2;
        
        gauss_img = gauss_filter(image, 1.2f);
        gradient(image, grad_sum_img, grad_dir_img);
        nonMaxima_img = nonMaxima_suppression<int, int>(grad_sum_img, grad_dir_img);
        hysterese_img = hysterese<int, int>(nonMaxima_img, 50.0f, 10.0f);

        hough_img0 = circle_houghTransformation(hysterese_img, ONE_CENT_R);
        int maxima_count = detectLocalMaxima(hough_img0, image, ONE_CENT_R);
        cout << "There were " << maxima_count << " one cent coins found in the image." << endl;

        hough_img1 = circle_houghTransformation(hysterese_img, TWO_CENT_R);
        maxima_count = detectLocalMaxima(hough_img1, image, TWO_CENT_R);
        cout << "There were " << maxima_count << " two cent coins found in the image." << endl;

        hough_img2 = circle_houghTransformation(hysterese_img, FIVE_CENT_R);
        maxima_count = detectLocalMaxima(hough_img2, image, FIVE_CENT_R);
        cout << "There were " << maxima_count << " five cent coins found in the image." << endl;

        savePGM(filename + "_hough0.pgm", hough_img0);
        savePGM(filename + "_hough1.pgm", hough_img1);
        savePGM(filename + "_hough2.pgm", hough_img2);
        savePGM(filename + "_marked.pgm", image);
    }

    return 0;
}

