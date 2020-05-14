//
//  mms_image_histogram_tools.hpp
//

#include <vector>

#ifndef mms_image_histogram_tools_hpp
#define mms_image_histogram_tools_hpp

namespace mms
{
	// Implementieren Sie die Verfahren der Histogrammdehnung 1) 
	// und der Histogrammlinearisierung (histogram equalization) 2). 

	// Berechnung des Histogramms 
	template<class PixelType, class HistogramDataType>
	void calculate_histogram(TImage<PixelType> f_image, std::vector<HistogramDataType>& hf) {
		PixelType min, max;
		getMinMax(min, max, f_image);
				
		if (hf.size() != ((max - min) + 1)) { return; }

		for (PixelType& pixel : f_image) {
			++hf.at(pixel);
		}
	}	
	
	// Histogramm Normalisierung
	template<class HistogramDataType>
	void normalize_histogram(std::vector<HistogramDataType>& hf, int pixelcount) {
		for (HistogramDataType& g : hf) {
			g = g / pixelcount;
		}
	}

	// Berechnung des kumulativen Histogramms
	template<class HistogramDataType>
	void calculate_cumulative_histogram(std::vector<HistogramDataType>& hf, std::vector<HistogramDataType>& c_hf) {
		if (hf.size() == c_hf.size()) {
			for (int g = 0; g < hf.size(); g++) {
				if (g == 0) {
					c_hf.at(g) = hf.at(g);
				}
				else {
					c_hf.at(g) = c_hf.at((g - 1)) + hf.at(g);
				}
			}
		}
		else {
			std::cout << "Histograms are not the same size." << std::endl;
		}
	}

	// Quantilsfunktion
	template<class HistogramDataType>
	HistogramDataType quantil(std::vector<HistogramDataType>& c_hf, float p) {
		HistogramDataType k = c_hf.size() - 1;

		for (int g = 0; g < k; g++) {
			if (c_hf.at(g) >= p * c_hf.at(k)) {
				return g;
			}
		}
		return -1;
	}
	
	// Intensitätsspreizung
	template<class PixelType, class HistogramDataType>
	void intensity_spread(float p_min, float p_max, std::vector<HistogramDataType>& c_hf, TImage<PixelType>& f_image) {
		PixelType min, max;
		getMinMax(min, max, f_image);
		HistogramDataType k = max;

		if (p_min >= p_max) { return; } // p_min should not be bigger than p_max

		HistogramDataType f_min = quantil(c_hf, p_min);
		HistogramDataType f_max = quantil(c_hf, p_max);

		for (PixelType& pixel : f_image) {
			float res = ((float)(pixel - f_min) / (float)(f_max - f_min));
			int i = k * res;
			if (i > max) { i = max; }
			if (i < min) { i = min; }
			pixel = i;
		}
	}

	//Histogramm-Dehnung
	template<class PixelType, class HistogramDataType>
	void histogram_stretching(TImage<PixelType>& f_image, float p_min, float p_max) {
		PixelType min, max;
		getMinMax(min, max, f_image);
		std::vector<HistogramDataType> hf(((max - min) + 1), 0);
		std::vector<HistogramDataType> c_hf(((max - min) + 1), 0);

		//Schritt 0: Histogramm berechnen
		calculate_histogram(f_image, hf);

		//Schritt 1: Histogramm kumulieren
		calculate_cumulative_histogram(hf, c_hf);

		//Schritt 2: Intensitätsspreizung
		intensity_spread(p_min, p_max, c_hf, f_image);
	}

	// Histogramm-Linearisierung
	template<class PixelType, class HistogramDataType>
	void histogram_equalization(TImage<PixelType>& f_image) {
		PixelType min, max;
		getMinMax(min, max, f_image);
		std::vector<HistogramDataType> hf(((max - min) + 1), 0);
		std::vector<HistogramDataType> c_hf(((max - min) + 1), 0);
		std::vector<HistogramDataType> result(((max - min) + 1), 0);
		std::vector<HistogramDataType> t(((max - min) + 1), 0);

		//Schritt 0: Histogramm berechnen und normalisieren
		calculate_histogram(f_image, hf);
		normalize_histogram(hf, f_image.getHeight() * f_image.getWidth());

		//Schritt 1: Histogramm kumulieren
		calculate_cumulative_histogram(hf, c_hf);

		//Schritt 2: Linearisierung durch die Abbildungsfunktion t(g) der Grauwerte g
		int k = c_hf.size() - 1;
		for (int g = 0; g < k; g++) {
			//Abbildungsfunktion t(g)
			t.at(g) = k * (float)c_hf.at(g) / (float)c_hf.at(k);

			//Linearisierung in Ergebnis-Histogramm festhalten
			result.at(g) = hf.at(t.at(g));
		}
	}
} // namespace mms

#endif