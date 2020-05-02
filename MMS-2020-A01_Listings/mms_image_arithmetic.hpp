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

}// namespace mms

#endif
