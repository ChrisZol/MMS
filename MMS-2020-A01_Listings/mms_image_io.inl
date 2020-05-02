#include "mms_image_io.hpp"
#include <fstream>
#include <memory>
#include <regex>

using namespace std;

template<class ImageType>
int maxVal(ImageType& f_image) {
	int maxVal = 0;

	try {
		for (int i = 0; i < f_image.getWidth(); i++) {
			for (int j = 0; j < f_image.getHeight(); j++) {

				if ((int)f_image(i, j) > maxVal) {
					maxVal = (int)f_image(i,j);
				}
			}
		}
		return maxVal;
	}
	catch (exception ex) {
		return false;
	}
}

template<class PixelType>
bool mms::readPGM(const std::string& f_filename, TImage<PixelType>& f_image)
{
	//Parsing PGM
	//1. Image Format
	//	P2 => Portable Graymap (ASCII)
	//	P5 => Portable Graymap (Binär)
	//2. Empty
	//3. Image Width
	//4. Empty
	//5. Image Height
	//6. Empty
	ifstream pgm_file(f_filename);
	string line;
	int format, width, height, max;
	smatch m;
	vector<string> vals;

	//Extract the header data
	while (getline(pgm_file, line)) {
		//if comment: continue
		while (regex_search(line, m, regex("(-?\\d+(\\.\\d+)*)"))) {
			vals.push_back(m.str(1));
			line = m.suffix().str();
		}
		if (vals.size() == 4) { break; }
	}

	format = stoi(vals.at(0));
	if (format != 2 && format != 5) { return false; }
	width = stod(vals.at(1));
	height = stod(vals.at(2));
	max = stod(vals.at(3));
	f_image = TImage<PixelType>(width, height);

	//Extract the image data
	try {
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				string val;
				pgm_file >> val;
				if (val != "") {
					f_image(i, j) = stoi(val);
				}
			}
		}
	}
	catch (exception ex) {
		return false;
	}

	return true;
}

template<class PixelType>
bool mms::savePGM(const std::string& f_filename, const TImage<PixelType>& f_image)
{
	ofstream pgm_file(f_filename);
	int c = 0;

	//Header of the file
	pgm_file << "P2\n# " << f_filename << "\n" << f_image.getWidth() << " " << f_image.getHeight() << "\n" << maxVal(f_image) << endl;

	//Image Data
	for (auto x : f_image) {
		pgm_file << toascii(x) << " ";

		//Adding a new line when the image width is reached
		c++;
		if (c == f_image.getWidth()) {
			pgm_file << endl;
			c = 0;
		}
	}
	return false;
}