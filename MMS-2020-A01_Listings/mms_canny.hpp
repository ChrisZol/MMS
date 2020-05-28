//
//  mms_image_arithmetic.hpp
//

#include <math.h>
#include <mutex>

#ifndef mms_canny_hpp
#define mms_canny_hpp
using namespace std;
const double PI = 3.141592653589793238463;
std::mutex lock1, lock2;

namespace mms
{
	template <class PixelType>
	void detectEdges(TImage<PixelType> f_image, string f_image_name) {
		TImage<PixelType> gauss_img;
		TImage<int> grad_sum_img(f_image.getWidth(), f_image.getHeight());
		TImage<int> grad_dir_img(f_image.getWidth(), f_image.getHeight());
		TImage<int> nonMaxima_img;
		TImage<int> hysterese_img;

		gauss_img = gauss_filter(f_image, 1.2f);
		gradient(f_image, grad_sum_img, grad_dir_img);
		nonMaxima_img = nonMaxima_suppression(grad_sum_img, grad_dir_img);
		hysterese_img = hysterese(nonMaxima_img, 60, 40);

		//Saving all the images for presentation purposes
		savePGM(f_image_name + "_gauss.pgm", gauss_img);
		savePGM(f_image_name + "_grad_sum.pgm", grad_sum_img);
		savePGM(f_image_name + "_grad_dir.pgm", grad_dir_img);
		savePGM(f_image_name + "_nonMaxima.pgm", nonMaxima_img);
		savePGM(f_image_name + "_hysterese.pgm", hysterese_img);
	}

	int gradient_dir(int dx, int dy) {
		return 180 * atan2(dy, dx) / PI;
	}

	int gradient_sum(int dx, int dy) {
		return sqrt(pow(dx, 2) + pow(dy, 2));
	}

	int base_direction(float direction) {
		if ((direction > 67.5f && direction < 112.5f) || (direction > -112.5f && direction < -67.5f)) {
			//rechts o. links
			return 0;
		}
		else if ((direction < -157.5 || direction > 157.5f) || (direction > -22.5f && direction < 22.5f)) {
			//oben o. unten
			return 90;
		}
		else if ((direction > 112.5f && direction < 157.5f) || (direction > 67.5f && direction < -22.5f)) {
			//rechts oben o. links unten
			return 135;
		}
		else if ((direction > 22.5f && direction < 67.5f) || (direction > -157.5f && direction < -112.5f)) {
			//rechts unten o. links oben
			return 45;
		}
	}

	template<class PixelType, class FilterPixelType>
	TImage<PixelType> convolution(TImage<PixelType> f_image, TImage<FilterPixelType> filter_mask) {
		TImage<PixelType> result_img(f_image.getWidth(), f_image.getHeight());
		int k = filter_mask.getWidth();

		for (int y = 0; y < f_image.getHeight(); y++) {
			for (int x = 0; x < f_image.getWidth(); x++) {
				double result = 0;
				for (int f_y = 0; f_y < k; f_y++) {
					for (int f_x = 0; f_x < k; f_x++) {
						PixelType curr;

						int _x = x - (f_x - (k / 2));
						int _y = y - (f_y - (k / 2));

						if (_x < 0 || _y < 0 || _x >= f_image.getWidth() || _y >= f_image.getHeight()) {
							curr = 0;
						}
						else {
							curr = f_image(_x, _y);
						}
						result += filter_mask(f_x, f_y) * (double)curr;
					}
				}
				result_img(x, y) = result;
			}
		}
		return result_img;
	}

	template <class PixelType>
	TImage<PixelType> gauss_filter(TImage<PixelType> f_image, float sigma) {
		//Gaußfilterung(2 Punkte)
		//	Implementieren Sie einen Gaußfilter, dessen Standardabweichung σ als
		//	Fließkommazahl einstellbar ist.Beachten Sie, dass die Größe der Filtermaske 6σ
		//	betragen und ungerade sein sollte, und dass die Filtersumme auf 1 normiert wird.
		//	Recherchieren Sie, was ein separierbarer Filter ist, und nutzen Sie für die
		//	Implementierung aus, dass der Gaußfilter separierbar ist.
		// Go(x,y) = 1/(2*pi*o^2) * e^((x^2+y^2)/o^2)
		double sum = 0;
		int k = 2 * ceil(3 * sigma) + 1;
		TImage<float> filter_mask(k, k);

		//Calculating Gauss-Filter-Mask
		for (int f_y = 0; f_y < k; f_y++) {
			for (int f_x = 0; f_x < k; f_x++) {
				int _x = f_x - (k / 2);
				int _y = f_y - (k / 2);

				float quot1 = (1.0f / (2.0f * PI * pow(sigma, 2.0f)));
				float quot2 = exp(-1 * ((pow(_x, 2) + pow(_y, 2.0f)) / pow(sigma, 2.0f)));
				filter_mask(f_x, f_y) = (quot1 * quot2) / pow(k, 2);
				sum += filter_mask(f_x, f_y);
			}
		}
		//Normalizing Filter Sum
		int quot = 1 / sum;
		for (float& pixel : filter_mask) {
			pixel *= quot;
		}

		//Convolution with Gauss-Filter
		return convolution(f_image, filter_mask);
	}

	template <class PixelType>
	void gradient(TImage<PixelType>& f_image, TImage<int>& grad_sum_img, TImage<int>& grad_dir_img) {
		//b) Gradientenbildung(2 Punkte)
		//	Implementieren Sie eine Gradientenberechnung, die als Ergebnis eines Eingabebildes
		//	jeweils ein Ergebnisbild für Gradientenbetrag und für Gradientenrichtung liefert.
		//Näherung des Gradienten:
		PixelType current, left_neighbour, above_neighbour;
		for (int y = 0; y < f_image.getHeight(); y++) {
			for (int x = 0; x < f_image.getWidth(); x++) {
				current = f_image(x, y);
				if (x == 0) {
					left_neighbour = f_image(x, y);
				}
				else {
					left_neighbour = f_image(x - 1, y);
				}
				if (y == 0) {
					above_neighbour = f_image(x, y);
				}
				else {
					above_neighbour = f_image(x, y - 1);
				}
				grad_sum_img(x, y) = gradient_sum(current - left_neighbour, current - above_neighbour);
				grad_dir_img(x, y) = gradient_dir(current - left_neighbour, current - above_neighbour);
			}
		}
	}

	template<class DestPixelType, class GradPixelType>
	TImage<DestPixelType> nonMaxima_suppression(TImage<GradPixelType>grad_sum_img, TImage<GradPixelType>grad_dir_img) {
		//c) Non - maxima - suppression(3 Punkte)
		//	Implementieren Sie eine Non - maxima - suppression, welche als Eingangsbilder ein Bild
		//	für Gradientenbetrag und ein Bild für Gradientenrichtung bekommt und als Ergebnis ein
		//	ausgedünntes Gradientenbild(Betrag) liefert.
		//- Ausgangsbild n(x, y)
		//- Weise alpha(x, y) eine Basisrichtung dk zu(allgemein
		//		aus vier möglichen)
		//- Falls | df(x, y) | größer als
		//	beide Nachbarn entlang
		//	Richtung dk, n(x, y) = | df(x, y) | , sonst n(x, y) = 0
		TImage<DestPixelType> n(grad_sum_img.getWidth(), grad_sum_img.getHeight());

		//grad_sum_img and grad_dir_img should be of same size
		if (grad_sum_img.getHeight() != grad_dir_img.getHeight() || grad_sum_img.getWidth() != grad_dir_img.getWidth()) { return n; }

		int prev = 0, next = 0;
		for (int y = 0; y < grad_sum_img.getHeight(); y++) {
			for (int x = 0; x < grad_sum_img.getWidth(); x++) {
				int dir = base_direction(grad_dir_img(x, y));
				if (dir == 0) {
					if (y == 0)
						prev = 0;
					else
						prev = grad_sum_img(x, y-1);
					if (y + 1 == grad_sum_img.getHeight())
						next = 0;
					else
						next = grad_sum_img(x, y+1);
				}
				else if (dir == 45) {
					if (y + 1 == grad_sum_img.getHeight() || x + 1 == grad_sum_img.getWidth())
						next = 0;
					else
						next = grad_sum_img(x + 1, y + 1);
					if (y == 0 || x == 0)
						prev = 0;
					else
						prev = grad_sum_img(x - 1, y - 1);
				}
				else if (dir == 90) {
					if (x == 0)
						prev = 0;
					else
						prev = grad_sum_img(x-1, y);
					if (x + 1 == grad_sum_img.getWidth())
						next = 0;
					else
						next = grad_sum_img(x+1, y);
				}
				else if (dir == 135) {
					if (x + 1 == grad_sum_img.getWidth() || y == 0)
						next == 0;
					else
						next = grad_sum_img(x + 1, y - 1);
					if (x == 0 || y + 1 == grad_sum_img.getHeight())
						prev = 0;
					else
						prev = grad_sum_img(x - 1, y + 1);
				}
				if (grad_sum_img(x, y) > prev && grad_sum_img(x,y) > next) {
					n(x, y) = grad_sum_img(x, y);
				}
				else {
					n(x, y) = 0;
				}
			}
		}
		return n;
	}

	template<class DestPixelType, class GradPixelType>
	TImage<DestPixelType> hysterese(TImage<GradPixelType>grad_image, const float TH, float TL) {
		//d) Hysterese(3 Punkte)
		//	Implementieren Sie eine Hysterese, welche als Eingabe ein Gradientbild(Betrag) und
		//	zwei Schwellwerte bekommt und als Ergebnis entsprechend den Schwellwerten ein Bild
		//	mit zusammengehörenden Kanten liefert.
		//- Verwendung von zwei Schwellwerten TH > TL
		//if (TH <= TL) { return null; }
		//- Generiere zwei Bilder(init.mit 0) auf Non - maxima - suppression
		//	Bild n(x, y) :
		TImage<DestPixelType> tH(grad_image.getWidth(), grad_image.getHeight());
		TImage<DestPixelType> tL(grad_image.getWidth(), grad_image.getHeight());
		//	tL(x, y) = n(x, y) > TL und tH(x, y) = n(x, y) > TH
		for (int y = 0; y < grad_image.getHeight(); y++) {
			for (int x = 0; x < grad_image.getWidth(); x++) {
				if (grad_image(x, y) > TL) {
					tL(x, y) = grad_image(x, y);
				}

				if (grad_image(x, y) > TH) {
					tH(x, y) = grad_image(x, y);
				}
			}
		}
		//- Lösche tH von tL :
		//	tL(x, y) = tL(x, y) - tH(x, y)
		subImageFromImage(tH, tL);

		//- Festlegung :
		//	-alle Pixel > 0 von tH(x, y) als starke Kanten
		//	- alle Pixel > 0 von tL(x, y) als schwache Kanten
		//- Schließe Lücken zwischen starken Kanten :
		//	1.Besuche das nächste Pixel p aus tH(x, y)
		//	2.Markiere in tL(x, y) Nachbarpixel aus N8(p) und alle Pixel einer zum
		//		Nachbarpixel gehörenden zusammenhängenden Kante als gültig
		//	3.Falls noch nicht alle Pixel aus tH(x, y) besucht, gehe zu 1.
		//- Ergebnis : Nur gültig markierte Pixel aus tL(x, y) und tH(x, y) als Kanten
		for (int y = 0; y < tH.getHeight(); y++) {
			for (int x = 0; x < tH.getWidth(); x++) {
				if (tH(x, y) > 0) {
					recursive_hysterese(tL, x, y);
				}
				else {
					tL(x, y) = 0;
				}
			}
		}

		return tL;
	}

	void recursive_hysterese(TImage<int>& tL, int x, int y) {
		tL(x, y) = 1;

		if (x + 1 < tL.getWidth()) {
			if (tL(x + 1, y) > 1) {
				recursive_hysterese(tL, x + 1, y);
			}
		}
		if (x - 1 >= 0){
			if (tL(x - 1, y) > 1) 
				recursive_hysterese(tL, x - 1, y);
		}
		if (y + 1 < tL.getHeight()) {
			if (tL(x, y + 1) > 1) {
				recursive_hysterese(tL, x, y + 1);
			}
		}
		if (y - 1 >= 0){
			if (tL(x, y - 1) > 1) 
				recursive_hysterese(tL, x, y - 1);
		}
		if (x + 1 < tL.getWidth() && y + 1 < tL.getHeight()) {
			if (tL(x + 1, y + 1) > 1) {
				recursive_hysterese(tL, x + 1, y + 1);
			}
		}
		if (x - 1 > 0 && y - 1 >= 0){
			if (tL(x - 1, y - 1) > 1) 
				recursive_hysterese(tL, x - 1, y - 1);
		}
		if (x + 1 != tL.getWidth() && y - 1 >= 0) {
			if (tL(x + 1, y - 1) > 1) {
				recursive_hysterese(tL, x + 1, y - 1);
			}
		}
		if (y + 1 != tL.getHeight() && x - 1 >= 0) {
			if (tL(x - 1, y + 1) > 1) {
				recursive_hysterese(tL, x - 1, y + 1);
			}
		}
	}
}// namespace mms

#endif
