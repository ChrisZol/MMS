#pragma once
//
//  mms_image_histogram_tools.hpp
//

#ifndef mms_image_histogram_tools_hpp
#define mms_image_histogram_tools_hpp

namespace mms
{
	template<class PixelType>
	void calculate_histogram(TImage<PixelType>& f_image) {
		// Berechnung des Histogramms
	}	
	
	template<class PixelType>
	void calculate_cumulative_histogram(TImage<PixelType>& f_image) {
		// Berechnung des kumulativen Histogramms
	}
		
	template<class PixelType>
	void intensity_spread(TImage<PixelType>& f_image) {
		// Quantilsfunktion und Intensitätsspreizung
	}


	template<class PixelType>
	void histogram_equalization(TImage<PixelType>& histogram) {
		// Linearisierung durch die Abbildungsfunktion t(g) der Grauwerte g
	}
} // namespace mms

#endif