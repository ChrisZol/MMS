//
//  mms_image_arithmetic.hpp
//

#ifndef mms_image_arithmetic_hpp
#define mms_image_arithmetic_hpp

namespace mms
{
    // Add a scalar value to each pixel of f_image.
    // NOTE: result is not checked whether it is in the pixel range (overflow possible).
    template<class PixelType>
	void addScalarToImage(const PixelType& f_add, TImage<PixelType>& f_image)
	{		
		for (PixelType& pixel : f_image) {
			pixel += f_add;
		}
	}

	// Subtract a scalar value from each pixel of f_image.
	// NOTE: result is not checked whether it is in the pixel range (overflow possible).
	template<class PixelType>
	void subScalarFromImage(const PixelType& f_sub, TImage<PixelType>& f_image)
	{
		for (PixelType& pixel : f_image) {
			pixel -= f_sub;
		}
	}

	// Divide each pixel of f_image by value f_div.
	template<class PixelType>
	void divImageByScalar(const PixelType& f_div, TImage<PixelType>& f_image)
	{
		if (f_div == 0) { return; } // Cannot divide by 0
		for (PixelType& pixel : f_image) {
			pixel /= f_div;
		}
	}

	// Multiply each pixel of f_image by value f_mul.
	template<class PixelType>
	void multImageByScalar(const PixelType& f_mul, TImage<PixelType>& f_image)
	{
		for (PixelType& pixel : f_image) {
			pixel *= f_mul;
		}
	}

	// Add pixelwise f_srcImage to f_destImage.
	// NOTE: result is not checked whether it is in the pixel range.
	// NOTE: image must be of same width and height.
	template<class PixelTypeSrc, class PixelTypeDest>
	void addImageToImage(const TImage<PixelTypeSrc>& f_srcImage, TImage<PixelTypeDest>& f_destImage)
	{
		if (f_srcImage.getHeight() == f_destImage.getHeight() && f_srcImage.getWidth() == f_destImage.getWidth()) {
			for (int i = 0; i < f_srcImage.getWidth(); i++) {
				for (int j = 0; j < f_srcImage.getHeight(); j++) {
					f_destImage(i, j) = f_destImage(i, j) + f_srcImage(i, j);
				}
			}
		}
		else {
			std::cout << "Source image and destination image must be of same length and width." << std::endl;
		}
	}

	// Subtract pixelwise f_srcImage to f_destImage.
    // NOTE: result is not checked whether it is in the pixel range.
    // NOTE: image must be of same width and height.
	template<class PixelTypeSrc, class PixelTypeDest>
	void subImageFromImage(const TImage<PixelTypeSrc>& f_srcImage, TImage<PixelTypeDest>& f_destImage)
	{
		if (f_srcImage.getHeight() == f_destImage.getHeight() && f_srcImage.getWidth() == f_destImage.getWidth()) {
			for (int i = 0; i < f_srcImage.getWidth(); i++) {
				for (int j = 0; j < f_srcImage.getHeight(); j++) {
					f_destImage(i, j) = f_destImage(i, j) - f_srcImage(i, j);
				}
			}
		}
		else {
			std::cout << "Source image and destination image must be of same length and width." << std::endl;
		}
	}

}// namespace mms

#endif
