#include "mms_image_io.hpp"
#include <fstream>
#include <memory>
#include <regex>

using namespace std;

template<class PixelType>
void parseBinary(ifstream& pgm_file, TImage<PixelType>& f_image) {
	// Copies all data into buffer
	size_t buffer_size = f_image.getWidth() * f_image.getHeight();
	char* buffer = new char[buffer_size];
	int c = 0;

	pgm_file.read(buffer, buffer_size);

	for (PixelType& pixel : f_image) {
		pixel = buffer[c];
		c++;
	}
}
template<class PixelType>
void parseASCII(ifstream& pgm_file, TImage<PixelType>& f_image) {
	//Extract the image data
	for (PixelType& pixel : f_image) {
			string val;
			pgm_file >> val;
			pixel = stoi(val);
	}
}

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
	ifstream pgm_file(f_filename, std::ios_base::binary);
	if (!pgm_file.good()) { return false; }
	string line;
	int format, width, height, max;
	smatch m,s;
	vector<string> vals;

	//Extract the header data
	while (getline(pgm_file, line)) {
		// if comment: continue
		regex_search(line, s, regex("\\S"));
		if (s.str(0) == "#") {
			continue; 
		}

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

	if (format == 2) {
		parseASCII(pgm_file, f_image);
	}
	else if (format == 5) {
		parseBinary(pgm_file, f_image);
	}
	else {
		cout << "Wrong file format." << endl;
		return false;
	}

	pgm_file.close();

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
		pgm_file << to_string(x) << " ";

		//Adding a new line when the image width is reached
		c++;
		if (c == f_image.getWidth()) {
			pgm_file << endl;
			c = 0;
		}
	}
	return false;
}