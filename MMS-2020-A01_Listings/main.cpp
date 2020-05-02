//
//  main.cpp
//

#include "mms_image.hpp"
#include "mms_image_arithmetic.hpp"
#include "mms_image_io.hpp"

#include <string>
#include <iostream>

using namespace mms;

int main(int argc, const char * argv[])
{
    using image_t = TImage<unsigned char>;
    image_t pgmImage;

    std::string filename("feep.pgm");
	
    readPGM(filename, pgmImage);

	addScalarToImage((unsigned char)10, pgmImage);

	savePGM(filename, pgmImage);

    pgmImage(0, 0);

    return 0;
}

