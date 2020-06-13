//
//  mms_classification.hpp
//

#ifndef mms_classification_hpp
#define mms_classification_hpp

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
using namespace std;

// Namespace for "Mustererkennung und Maschinelles Sehen"
namespace mms {
	enum Color {
		GREEN,
		YELLOW,
		RED, 
		ORANGE,
		WHITE,
		BLUE_RED,
		YELLOW_RED,
	};

	enum Form {
		ELEPHANT,
		DEVIL,
		BEAR,
		TURTLE,
		DINOSAUR,
		HIPPO, 
		SCHNULLER,
		UNKNOWN
	};

	class Segment_Class {
	public:
		Segment_Class();
		Segment_Class(vector<cv::Point> conture);

		void identifyColor(cv::Mat);
		void identifyForm(cv::Mat);

		Color getColor();
		Form getForm();

	private:
		vector<cv::Point> conture;
		Color color;
		Form form;
		float rd;
		float cd;
		float elongation;

		void calculate_elongation(cv::Mat);
		cv::Point focus_point();
		float avg_contureDistance();
		float avg_radialDistance();
		float radialDistance(cv::Point, cv::Point);
		vector<float> normalized_radialDistances(std::vector<float>);
		vector<double> normalized_conture_moment();
		cv::Vec3f rgb_to_hsv(cv::Vec3f rgb);
	};

	class Classification {
	public:

		// Constructor, allocates an image of zero (0 x 0) size
		Classification();

		//Default copy/move contructor/assignment operator
		Classification(const Classification&) = default;
		Classification(Classification&&) = default;
		Classification& operator=(const Classification&) = default;
		Classification& operator=(Classification&&) = default;

		void segmentation(string filename);
		void classification();

		cv::Mat getImageRGB();
		cv::Mat getImageBW();
		cv::Mat getImageID();

	private:
		cv::Point nextNeighbour(cv::Point, cv::Point);
		void conture_to_point_sequence(cv::Point b0, int id);

		cv::Mat img_rgb;
		cv::Mat img_bw;
		cv::Mat img_id;
		vector<mms::Segment_Class> segments;
	};

}//end namespace mms

#include "mms_classification.inl"

#endif
