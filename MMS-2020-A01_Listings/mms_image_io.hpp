#ifndef mms_image_io_hpp
#define mms_image_io_hpp
#include "mms_image.hpp"
#include <string>
namespace mms
{
    template <class PixelType>
    bool readPGM(const std::string& f_filename, TImage<PixelType>& f_image);
    template <class PixelType>
    bool savePGM(const std::string& f_filename, const TImage<PixelType>& f_image);

} // end namespace mms
#include "mms_image_io.inl"
#endif