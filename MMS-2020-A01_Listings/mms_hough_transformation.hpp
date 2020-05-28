//
//  mms_hough_transformation.hpp
//

#include <vector>

#ifndef mms_hough_transformation_hpp
#define mms_hough_transformation_hpp

namespace mms
{
	//generelle Idee für Algorithmus :
	//	-Zerteile(Diskretisiere) Hough - Raum in Zellen A(αi, di)
	//	- Nutze Zellen als Akkumulator(initial alle A(αi, di) = 0)
	//	- Für jeden Punkt(xp, yp) einer Linie(mit di(αi) = xp cos(αi) + yp sin(αi)) :
	//		• inkrementiere : A(αi, di) = A(αi, di) + 1
	//	- Finde lokale Maxima in A

	template <class PixelType>
	TImage<int> circle_houghTransformation(TImage<PixelType> edge_img, float r) {
		PixelType min, max;
		getMinMax(min, max, edge_img);
		TImage<int> hough_img(edge_img.getWidth(), edge_img.getHeight());

		for (int y = 0; y < edge_img.getHeight(); y++) {
			for (int x = 0; x < edge_img.getWidth(); x++) {
				if (edge_img(x,y) == max) {
					drawHoughCircle(hough_img, x, y, r);
				}
			}
		}

		return hough_img;
	}

	template <class PixelType>
	void drawHoughCircle(TImage<PixelType>& hough_img, int cx, int cy, float r) {
		int alpha = 0;
		while (alpha < 360) {
			//Punkt auf Kreisrand berechnen: P(cos(alpha), sin(alpha))
			float cosinus = cos(alpha * (PI / 180)), sinus = sin(alpha * (PI / 180));
			int x = cx + (cosinus * r), y = cy + (sinus * r);

			if (!(x < 0 || x >= hough_img.getWidth() || y < 0 || y >= hough_img.getHeight())) {
				hough_img(x, y) += 1; // Akkumulieren
			}
			alpha+=1;
		}
	}


	template <class PixelType, class DestPixelType>
	int detectLocalMaxima(TImage<PixelType>& hough_img, TImage<DestPixelType>& image_to_mark, int r) {
		PixelType min, total_max, maxima_count = 0;
		DestPixelType mark_min, mark_max;
		getMinMax(min, total_max, hough_img);
		getMinMax(mark_min, mark_max, image_to_mark);
		PixelType max = std::numeric_limits<DestPixelType>::min();
		int max_x = 0, max_y = 0;
		int x = (int)(sqrt(hough_img.getWidth()) + 0.5f);
		int y = (int)(sqrt(hough_img.getHeight()) + 0.5f);

		for (int j = 0; j < y; j++) {
			for (int i = 0; i < x; i++) {
				for (int img_y = j * y; img_y < (j + 1)*y; img_y++) {
					for (int img_x = i * x; img_x < (i+1)*x; img_x++) {
						if (img_x >= hough_img.getWidth() || img_y >= hough_img.getHeight()) { continue; }
						if (hough_img(img_x, img_y) > max) {
								max = hough_img(img_x, img_y);
								max_x = img_x;
								max_y = img_y;
						}
					}
				}
				//Schwellwert
				if (max > total_max * 0.70f) {
					drawX(image_to_mark, max_x, max_y, mark_max, r);
					maxima_count++;
				}
				max = std::numeric_limits<DestPixelType>::min();
			}
		}

		return maxima_count;
	}

	template <class PixelType>
	void drawX(TImage<PixelType>& image, int x, int y, PixelType max, int r) {
		for (int i = 0; i < r; i++) {
			if (!(x + i >= image.getWidth())) {
				image(x + i, y) = max;
			}
			if (!(y + i >= image.getHeight())) {
				image(x, y + i) = max;
			}
			if (!(x - i < 0 )) {
				image(x - i, y) = max;
			}
			if (!(y - i < 0)) {
				image(x, y - i) = max;
			}
		}
	}

} // namespace mms

#endif