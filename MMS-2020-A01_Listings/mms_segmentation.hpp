//
//  mms_hough_transformation.hpp
//

#include <vector>

#ifndef mms_segmentation_hpp
#define mms_segmentation_hpp

namespace mms
{
	template <class PixelType>
	void segmentation(bool otsu, TImage<PixelType>& img, TImage<PixelType>& segment_img) {
		float T;
		if (otsu) {
			T = otsu(local_img);
		}
		else {
			iterative_selection(local_img, 0);
		}

		for (int y = 0; y < img.getHeight(); y++) {
			for (int x = 0; x < img.getWidth(); x++) {
				if (img(x, y) > T) {
					segment_img(x, y) = 1;
				}
				else {
					segment_img(x, y) = 0;
				}
			}
		}
	}
	template <class PixelType>
	void segmentation(bool otsu_seg, TImage<PixelType>& img, TImage<PixelType>& segment_img, int local_width, int local_height) {
		TImage<PixelType> local_img(local_width, local_height);
		TImage<PixelType> segments(img.getWidth() / local_width, img.getHeight() / local_height);
		int s_x = 0, s_y = 0;

		for (int y = 0; y < img.getHeight(); y+=local_height) {
			for (int x = 0; x < img.getWidth(); x+=local_width) {
				cropImage(img, local_img, x, y);
				float T;

				if (otsu_seg) {
					T = otsu(local_img);
				}
				else {
					T = iterative_selection(local_img, 0);
				}

				segments(s_x, s_y) = T;
				if(s_x < segments.getWidth() - 1)
					s_x++;
				else {
					s_x = 0;
					s_y++;
				}

				for (int j = y; j < y + local_height; j++) {
					for (int i = x; i < x + local_width; i++) {
						if (img(i,j) > T) {
							segment_img(i,j) = 1;
						}
						else {
							segment_img(i,j) = 0;
						}
					}
				}
			}
		}
		interpolation(img, segment_img, segments);
	}

	template <class PixelType>
	unsigned int iterative_selection(TImage<PixelType> img, int delta_T) {	
		PixelType min, max;
		getMinMax(min, max, img);
		//Mittelwert
		float T = 0;
		vector<int> hf(max + 1);
		float Tnew;
		calculate_histogram(img, hf);

		for (int i = 0; i < hf.size(); i++) {
			T += i * hf.at(i);
		}

		T /= img.getHeight() * img.getWidth();

		while(1) {
			float Ta = 0;
			float Tb = 0;
			int suma = 0, sumb = 0;
			for (int i = 0; i < hf.size(); i++) {
				if (i < T) {
					suma += hf.at(i);
					Ta += i * hf.at(i);
				}
				else {
					sumb += hf.at(i);
					Tb += i * hf.at(i);
				}
			}
			Ta /= suma;
			Tb /= sumb;

			Tnew = (Ta + Tb) / 2;
			if ((Tnew - T) > delta_T) {
				T = Tnew;
			}
			else {
				break;
			}
		}

		return T;
	}


	template <class PixelType>
	float otsu(TImage<PixelType> img) {
		PixelType min, max;
		getMinMax(min, max, img);
		// 1. Berechne normalisiertes Histogramm p des Bildes
		vector<int> hf(max + 1);
		vector<float> p(max + 1);

		calculate_histogram(img, hf);
		normalize_histogram(hf, img.getHeight() * img.getWidth(), p);

		// 2. Berechne Mittelwert des Gesamtbildes mf
		float mf = 0;

		for (int i = 0; i < p.size(); i++) {
			mf += (i * p.at(i));
		}

		// 3. Berechne für jedes t€[0,k]
		vector<float> P1(mf);
		vector<float> m(mf);
		vector<float> ob2(mf);
		float tmax = std::numeric_limits<float>::min();
		float tmax_val = std::numeric_limits<float>::min();

		for (int t = 0; t < (int)mf; t++) {
			for (int i = 0; i <= t; i++) {
				//		a) die Summen für P1(t)
				P1.at(t) += p.at(i);

				//		b) die Mittelwerte m(t)
				m.at(t) += (i * p.at(i));
			}
			//		c) die between class Varianz
			float quot1 = pow(mf * P1.at(t) - m.at(t), 2);
			float quot2 = P1.at(t) * (1 - P1.at(t));
			ob2.at(t) = quot1 / quot2;
			if (ob2.at(t) > tmax_val) {
				tmax_val = ob2.at(t);
				tmax = t;
			}
		}

		return tmax;
	}

	template <class PixelType>
	void interpolation(TImage<PixelType>& img, TImage<PixelType>& segment_img, TImage<PixelType> segments) {
		int segment_width = img.getWidth() / segments.getWidth();
		int segment_height = img.getHeight() / segments.getHeight();

		for (int y = 0; y < img.getHeight(); y++) {
			for (int x = 0; x < img.getWidth(); x++) {
				if (x != 0 && x % segment_width == 0 && y != 0 && y % segment_height == 0) {
					int xmyn = segments((x / segment_width) - 1, (y / segment_height));
					int xm1yn = segments(x / segment_width , (y / segment_height));
					int xmyn1 = segments((x / segment_width) - 1,( y / segment_height) - 1);
					int xm1yn1 = segments(x / segment_width, (y / segment_height) - 1);

					int xm1 = x + (segment_width / 2);
					int xm = x - (segment_width / 2);
					int yn = y + (segment_height / 2);
					int yn1 = y - (segment_height / 2);

					for (int s_y = y - (segment_height / 2); s_y < y + (segment_height / 2); s_y++) {
						for (int s_x = x - (segment_width / 2); s_x < x + (segment_width / 2); s_x++) {
							int seg_x = segment_width / s_x;
							int seg_y = segment_height / s_y;

							int xyn1 = ((float)((float)(xm1 - s_x) / (float)(xm1 - xm)) * xmyn1) + ((float)((float)(s_x - xm) / (float)(xm1 - xm)) * xm1yn1);
							int xyn = ((float)((float)(xm1 - s_x) / (float)(xm1 - xm)) * xmyn) + ((float)((float)(s_x - xm) / (float)(xm1 - xm)) * xm1yn);
							int xy = ((float)((float)(yn1 - s_y) / (float)(yn1 - yn)) * xyn) + ((float)((float)(s_y - yn) / (float)(yn1 - yn)) * xyn1);

							if (img(s_x, s_y) > xy) {
								segment_img(s_x, s_y) = 1;
							}
							else {
								segment_img(s_x, s_y) = 0;
							}
						}
					}
				}
			}
		}
	}

} // namespace mms

#endif