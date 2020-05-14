//
//  mms_image_tools.hpp
//
#include <stdio.h>
#ifndef mms_image_tools_hpp
#define mms_image_tools_hpp

#include "mms_image.hpp"

namespace mms {

    // Copy image data of f_srcImage to f_destImage
    // NOTE: image must be of same width and height.
	template<class PixelTypeSrc, class PixelTypeDest>
    void copyImage(const TImage<PixelTypeSrc>& f_srcImage, TImage<PixelTypeDest>& f_destImage)
    {
        if (f_srcImage.getHeight() == f_destImage.getHeight() && f_srcImage.getWidth() == f_destImage.getWidth()) {
            std::copy(f_srcImage.begin(), f_srcImage.end(), f_destImage.begin());
        }
        else {
            std::cout << "Source image and destination image must be of same length and width." << std::endl;
        }
    }

    // Fill image with data of f_fill.
    template<class PixelType>
    void fillImage(PixelType f_fill, TImage<PixelType>& f_image)
    {
        for (PixelType& pixel : f_image) {
            pixel = f_fill;
        }
    }
    
    // Clamp pixel values of image f_image to range given by f_min, f_max.
	template<class PixelType>
    void clampImage(PixelType f_min, PixelType f_max, TImage<PixelType>& f_image)
    {
        for (PixelType& pixel : f_image) {
            if (pixel > f_max) {
                pixel = f_max;
            }
            if (pixel < f_min) {
                pixel = f_min;
            }
        }
    }
    
    // Return max. and min. pixel values of f_image in f_min, f_max
	template<class PixelType>
    void getMinMax(PixelType& f_min, PixelType& f_max, const TImage<PixelType>& f_image)
    {
        PixelType maxVal = std::numeric_limits<PixelType>::min();
        PixelType minVal = std::numeric_limits<PixelType>::max();

        for (PixelType pixel : f_image) {
            if (pixel > maxVal) {
                maxVal = pixel;
            }
            if (pixel < minVal) {
                minVal = pixel;
            }
        }
        f_max = maxVal;
        f_min = minVal;
    }
    
    
} //namespace mms
#endif
