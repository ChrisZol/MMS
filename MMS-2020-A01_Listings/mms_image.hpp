//
//  mms_image.hpp
//

#ifndef mms_image_hpp
#define mms_image_hpp

#include <memory>
#include <vector>

// Namespace for "Mustererkennung und Maschinelles Sehen"
namespace mms {

    template <class PixelType, class AllocatorType = std::allocator<PixelType> >
    class TImage {
    public:
        // Member types
		using allocator_type = AllocatorType;
		using pixel_type = PixelType;
		using iterator = typename std::vector<pixel_type, allocator_type>::iterator;
		using const_iterator =  typename std::vector<pixel_type, allocator_type>::const_iterator;        
        
        // Constructor, allocates an image of zero (0 x 0) size
		TImage(const allocator_type& f_allocator = allocator_type());
        // Constructor, allocates an image of f_width*f_height
        explicit TImage(int f_width, int f_height, const allocator_type& f_allocator = allocator_type());  

		//Default copy/move contructor/assignment operator
		TImage(const TImage&) = default;
		TImage(TImage&&) = default;
		TImage& operator=(const TImage&) = default;
		TImage& operator=(TImage&&) = default;

		// Return a (const) reference to the pixel at position f_x and f_y.
		// NOTE: position is not checked if in image.
		pixel_type& operator()(int f_x, int f_y);
        const pixel_type& operator()(int f_x, int f_y) const;    

		//Iterators
		iterator begin();
		const_iterator begin() const;	
		iterator end();
		const_iterator end() const;

		// Resize image to f_width and f_height. If the object already contains data, these are deleted.
		void resize(int f_width, int f_height);

		// Return height of image.
		int getHeight() const;
		// Return width of image.
		int getWidth() const;	

    private:
        // Width and height of image.
        int m_width, m_height;

		// Data of image.
		std::vector<pixel_type, allocator_type> m_data;
};
    
}//end namespace mms

#include "mms_image.inl"

#endif
