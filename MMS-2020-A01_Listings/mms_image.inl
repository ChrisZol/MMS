#include "mms_image.hpp"
using namespace mms;

// ------------ //
// Constructors //
// ------------ //

template<class PixelType, class AllocatorType>
inline TImage<PixelType, AllocatorType>::TImage(const AllocatorType& f_allocator)
	: m_width(0), m_height(0), m_data(vector<PixelType>(f_allocator))
{
	m_data.assign(0, 0);
}

template<class PixelType, class AllocatorType>
inline TImage<PixelType, AllocatorType>::TImage(int f_width, int f_height, const AllocatorType& f_allocator)
	: m_width(f_width), m_height(f_height), m_data(vector<PixelType>(f_allocator))
{
	m_data.assign(f_width * f_height, 0);
}

// --------- //
// Operators //
// --------- //

template<class PixelType, class AllocatorType>
inline PixelType& TImage<PixelType, AllocatorType>::operator()(int f_x, int f_y)
{
	// Return a reference to the pixel at position f_x and f_y.
	if (!m_data.empty()) {
		int pos = (f_x * m_height) + f_y;
		return m_data.at(pos);
	}
}

template<class PixelType, class AllocatorType>
inline const PixelType& TImage<PixelType, AllocatorType>::operator()(int f_x, int f_y) const
{
	// Return a reference to the pixel at position f_x and f_y.
	if (!m_data.empty()) {
		int pos = (f_x * m_height) + f_y;
		return m_data.at(pos);
	}
}

// ------------------ //
// Iterator Functions //
// ------------------ //

template<class PixelType, class AllocatorType>
inline typename std::vector<PixelType, AllocatorType>::iterator mms::TImage<PixelType, AllocatorType>::begin()
{
	return m_data.begin();
}

template<class PixelType, class AllocatorType>
inline typename std::vector<PixelType, AllocatorType>::const_iterator mms::TImage<PixelType, AllocatorType>::begin() const
{
	return m_data.begin();
}

template<class PixelType, class AllocatorType>
inline typename std::vector<PixelType, AllocatorType>::iterator TImage<PixelType, AllocatorType>::end()
{
	return m_data.end();
}

template<class PixelType, class AllocatorType>
inline typename std::vector<PixelType, AllocatorType>::const_iterator TImage<PixelType, AllocatorType>::end() const
{
	return m_data.end();
}

// ------- //
// Getters //
// ------- //

template<class PixelType, class AllocatorType>
inline int TImage<PixelType, AllocatorType>::getHeight() const
{
	return m_height;
}

template<class PixelType, class AllocatorType>
inline int TImage<PixelType, AllocatorType>::getWidth() const
{
	return m_width;
}


// --------- //
// Methods	 //
// --------- //

template<class PixelType, class AllocatorType>
inline void TImage<PixelType, AllocatorType>::resize(int f_width, int f_height)
{
	//Data is cleared
	m_data.clear();

	//Resizing
	m_width = f_width;
	m_height = f_height;
}