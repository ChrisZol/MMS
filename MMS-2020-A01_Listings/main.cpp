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
    TImage<int> result_image(480, 615);
    vector<TImage<unsigned char>> images;
    
    // TODO:
    // c.) -0,5 P.: Bitte keine Magic number verwenden, wie z.B. die 10 für die Anzahl der Bilder.
    // Wenn sich nun die Anzahl der Bilder in der Aufgabenstellung ändert, muss an zwei Stellen etwas verändert werden (Z. 20 und 30).
    // Das kann man auch mal schnell vergessen (gerade in größeren Programmen), sodass es zu einem fehlerhaften Programm führt.

    for (int i = 0; i < 10; i++) {
        TImage<unsigned char> image;
        std::string filename = "../MMS-2020-A02c_Daten/applesNoise" + std::to_string(i) + ".pgm";
        if (readPGM(filename, image)) {
            images.push_back(image);
        }
        mms::addImageToImage(image, result_image);
    }

    // TODO:
    // -1 P.: Das "result_image" darf nicht geclampt werden (Z. 29). 
    // Durch die fehlerhafte Implementierung der Funktion (s.o) hast du Glück, 
    // dass das Ausgabebild nach der Division richtig "aussieht", jedoch einen falschen Bildinhalt hat.
    // Die Werte liegen hier zwischen 0 und 28, was auch nicht den Mittelwerten der Eingabebilder entspricht.
    // Bei einer korrekten Implementierung von clampImage() würde das Vorgehen nicht funktionieren.

    clampImage(0, 255, result_image);
    divImageByScalar<int>(10, result_image);
    savePGM("../MMS-2020-A02c_Daten/applesNoise_result.pgm", result_image);

    return 0;
}

