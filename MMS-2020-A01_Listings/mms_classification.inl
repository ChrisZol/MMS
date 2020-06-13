#include "mms_classification.hpp"
#define stringify( name ) # name

using namespace mms;
using namespace cv;
using namespace std;


const char* formNames[] =
{
stringify(ELEPHANT),
stringify(DEVIL),
stringify(BEAR),
stringify(TURTLE),
stringify(DINOSAUR),
stringify(HIPPO),
stringify(SCHNULLER),
stringify(UNKNOWN)
};

const char* colorNames[] =
{
stringify(GREEN),
stringify(YELLOW),
stringify(RED),
stringify(ORANGE),
stringify(WHITE),
stringify(BLUE_RED),
stringify(YELLOW_RED)
};

void vPrintError(Form form)
{
    cout << formNames[form] << endl;
}

inline mms::Classification::Classification()
{
}

inline void mms::Classification::segmentation(string filename)
{
    cv::Mat im_gray = imread("../MMS-2020-A07_Bild.jpg", CV_32F);
    cv::Mat im_rgb = imread("../MMS-2020-A07_Bild.jpg");
    cv::Mat img_equal;

    img_rgb = im_rgb.clone();

    // Higher Contrast for better thresholding
    double alpha = 2.3; /*< Simple contrast control */
    int beta = 0;       /*< Simple brightness control */
    for (int y = 0; y < im_rgb.rows; y++) {
        for (int x = 0; x < im_rgb.cols; x++) {
            for (int c = 0; c < im_rgb.channels(); c++) {
                img_rgb.at<Vec3b>(y, x)[c] =
                    saturate_cast<uchar>(alpha * im_rgb.at<Vec3b>(y, x)[c] + beta);
            }
        }
    }

    // RGB to GRAYSCALE
    cvtColor(img_rgb, im_gray, COLOR_RGB2GRAY);


    //medianBlur(im_gray, im_gray, 5);

    equalizeHist(im_gray, img_equal);

    //• die im Bild unten dargestellten Objekte zu segmentieren und das Ergebnis der
    //    Segmentierung mit morphologischen Operationen dahingehend zu verbessern, dass
    //    jedes Objekt genau eine ID erhält, (5 Punkte)

    //1) Adaptive Thresholding
    cv::adaptiveThreshold(img_equal, img_bw, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 201, 15);

    //2) Morphological Operation
    Mat morph_img;

    Mat kernel = Mat(7, 5, CV_32F);
    cv::morphologyEx(img_bw, morph_img, MORPH_CLOSE, kernel);

    kernel = Mat(12, 12, CV_32F);
    cv::morphologyEx(morph_img, morph_img, MORPH_OPEN, kernel);

    //3) Region Labelling

    Mat label_img;

    int nLabels = connectedComponents(morph_img, label_img, 8);
    std::vector<Vec3b> colors(nLabels);
    colors[0] = Vec3b(0, 0, 0);//background
    int labelColor = 255 / nLabels;

    for (int label = 1; label < nLabels; ++label) {
        colors[label] = Vec3b(labelColor, labelColor, labelColor);
        labelColor += (255 / nLabels);
    }
    Mat dst(img_bw.size(), CV_8UC3);
    for (int r = 0; r < dst.rows; ++r) {
        for (int c = 0; c < dst.cols; ++c) {
            int label = label_img.at<int>(r, c);

            Vec3b& pixel = dst.at<Vec3b>(r, c);
            pixel = colors[label];
        }
    }

      //• und zu den segmentierten Objekten Merkmale zu berechnen, die eine Zuordnung nach
      //    Farbe und Form erlauben.Überlegen Sie Sich geeignete Merkmale. (5 Punkte)

    imwrite("../MMS-2020-A07_Bild_rgb.jpg", img_rgb);
    imwrite("../MMS-2020-A07_Bild_gray.jpg", im_gray);
    imwrite("../MMS-2020-A07_Bild_thresh.jpg", img_bw);
    imwrite("../MMS-2020-A07_Bild_morph.jpg", morph_img);
    imwrite("../MMS-2020-A07_Bild_fill.jpg", dst);

    img_id = dst.clone();

    // Conture to Point Sequence
    int id = 0;
    for (int i = 0; i < nLabels; i++) {
        for (int r = 0; r < dst.rows; ++r) {
            for (int c = 0; c < dst.cols; ++c) {
                if (dst.at<Vec3b>(r, c)[0] > id) {
                    Vec3b val = dst.at<Vec3b>(r, c);
                    id = val[0];
                    conture_to_point_sequence(Point(c, r), id);
                }
            }
        }
    }

    classification();
}

inline void mms::Classification::classification()
{
    int id = 1;

    for (auto& segment : segments) {
        segment.identifyColor(img_rgb);
        segment.identifyForm(img_bw);
        Color color = segment.getColor();
        Form form = segment.getForm();

        cout << "Segment ID: " << id << endl;
        cout << "Segment Form: " << formNames[form] << endl;
        cout << "Segment Color: " << colorNames[color] << endl;
        cout << endl;
        id++;
    }

    cin.get();
}

inline cv::Mat mms::Classification::getImageRGB()
{
    return img_rgb;
}

inline cv::Mat mms::Classification::getImageBW()
{
    return img_bw;
}

inline cv::Mat mms::Classification::getImageID()
{
    return cv::Mat();
}


inline void mms::Classification::conture_to_point_sequence(cv::Point b0, int id)
{
    std::vector<Point> conture;
    Point b, c, b1(-1, -1), c1, nk(-1, -1);
    Point prev = Point(b0.x - 1, b0.y), current = b0;
    
    while (b1.x == -1) {
        current = nextNeighbour(b0, prev);
        if (img_id.at<Vec3b>(current)[0] >= id) {
            b1 = current;
            c1 = prev;
        }
        prev = current;
    }

    b = b1;
    c = c1;

    while (b != b0 && nk != b1) {
        nk = Point(-1, -1);
        prev = c;

        while (nk.x == -1) {
            current = nextNeighbour(b, prev);
            if (img_id.at<Vec3b>(current)[0] >= id) {
                nk = current;
                c = prev;
            }
            prev = current;
        }
        b = nk;
        conture.push_back(b);
    }
    segments.push_back(Segment_Class(conture));
}

inline mms::Segment_Class::Segment_Class()
{
}
inline mms::Segment_Class::Segment_Class(std::vector<Point> conture) : conture(conture)
{
}

inline void mms::Segment_Class::identifyColor(Mat img)
{
    Vec3b avg_color;
    float r = 0, g = 0, b = 0;

    Point focus = focus_point();
    for (auto point : conture) {
        r += img.at<Vec3b>(point)[2];
        g += img.at<Vec3b>(point)[1];
        b += img.at<Vec3b>(point)[0];
    }
    r += img.at<Vec3b>(focus)[2] * 10;
    g += img.at<Vec3b>(focus)[1] * 10;
    b += img.at<Vec3b>(focus)[0] * 10;

    avg_color[0] = r / ((float)conture.size() + 10);
    avg_color[1] = g / ((float)conture.size() + 10);
    avg_color[2] = b / ((float)conture.size() + 10);

    Vec3b hsv = rgb_to_hsv(avg_color);
  
    // H ~50-75 YELLOW
    if (hsv[0] > 40 && hsv[0] < 90) {
        color = YELLOW;
    }
    // H ~120-160 GREEN
    else if (hsv[0] >= 90 && hsv[0] < 160) {
        color = GREEN;
    }
    // H ~315-345 ORANGE
    else if (hsv[0] > 15 && hsv[0] < 40) {
        color = ORANGE;
    }
    // H ~30-345 RED
    else if ((hsv[0] >= 0 && hsv[0] < 15) || (hsv[0] > 345) ) {
        color = RED;
    }
    // H ~105-165 BLUE
    else if (hsv[0] > 170 && hsv[0] < 280) {
        color = BLUE_RED;
    }
    else {
        color = WHITE;
    }
    // S < 0,2 WHITE
    //if (hsv[1] > 0.2) {
    //    color = WHITE;
    //}
}

inline void mms::Segment_Class::identifyForm(Mat img_bw)
{
    calculate_elongation(img_bw);
    
    rd = avg_contureDistance();
    cd = avg_radialDistance();

    if (rd > 4.1 * 0.9 && rd < 4.1 * 1.1 && elongation > 2.0 * 0.9 && elongation < 2.0 * 1.1 && cd > 0.72 * 0.95 && cd < 0.72 * 1.05) {
        form = BEAR;
    }
    else if (rd > 3.5 * 0.9 && rd < 3.5 * 1.1 && elongation > 1.7 * 0.9 && elongation < 1.7 * 1.1 && cd > 0.7 * 0.95 && cd < 0.7 * 1.05) {
        form = TURTLE;
    }
    else if (rd > 4.4 * 0.9 && rd < 4.4 * 1.1 && elongation > 1.9 * 0.9 && elongation < 1.9 * 1.1 && cd > 0.65 * 0.95 && cd < 0.65 * 1.05) {
        form = SCHNULLER;
    }
    else if (rd > 4.0 * 0.9 && rd < 4.0 * 1.1 && elongation > 2.4 * 0.9 && elongation < 2.4 * 1.1 && cd > 0.62 * 0.95 && cd < 0.62 * 1.05) {
        form = DINOSAUR;
    }
    else if (rd > 3.7 * 0.9 && rd < 3.7 * 1.1 && elongation > 2.0 * 0.9 && elongation < 2.0 * 1.1 && cd > 0.67 * 0.95 && cd < 0.67 * 1.05) {
        form = HIPPO;
    }
    else if (rd > 3.8 * 0.9 && rd < 3.8 * 1.1 && elongation > 2.2 * 0.9 && elongation < 2.2 * 1.1 && cd > 0.68 * 0.95 && cd < 0.68 * 1.05) {
        form = DEVIL;
    }
    else if (rd > 3.7 * 0.9 && rd < 3.7 * 1.1 && elongation > 1.6 * 0.9 && elongation < 1.6 * 1.1 && cd > 0.72 * 0.95 && cd < 0.72 * 1.05) {
        form = ELEPHANT;
    }
    else {
        form = UNKNOWN;
    }
}

inline Color mms::Segment_Class::getColor()
{
    return color;
}

inline Form mms::Segment_Class::getForm()
{
    return form;
}

inline cv::Point mms::Classification::nextNeighbour(cv::Point pixel_pos, cv::Point c)
{
    Point neighbour;
    Point offset = Point(c.x - pixel_pos.x, c.y - pixel_pos.y);

    if (offset == Point(-1, 0)) {
        neighbour = Point(pixel_pos.x - 1, pixel_pos.y - 1);
    }
    else if (offset == Point(-1, -1)) {
        neighbour = Point(pixel_pos.x, pixel_pos.y - 1);
    }
    else if (offset == Point(0, -1)) {
        neighbour = Point(pixel_pos.x + 1, pixel_pos.y - 1);
    }
    else if (offset == Point(1, -1)) {
        neighbour = Point(pixel_pos.x + 1, pixel_pos.y);
    }
    else if (offset == Point(1, 0)) {
        neighbour = Point(pixel_pos.x + 1, pixel_pos.y + 1);
    }
    else if (offset == Point(1, 1)) {
        neighbour = Point(pixel_pos.x, pixel_pos.y + 1);
    }
    else if (offset == Point(0, 1)) {
        neighbour = Point(pixel_pos.x - 1, pixel_pos.y + 1);
    }
    else if (offset == Point(-1, 1)) {
        neighbour = Point(pixel_pos.x - 1, pixel_pos.y);
    }
    return neighbour;
}

inline void mms::Segment_Class::calculate_elongation(Mat img_id)
{
    RotatedRect minRect;
    minRect = minAreaRect(conture);

    Point2f rect_points[4];
    minRect.points(rect_points);

    float width = radialDistance((Point)rect_points[0], (Point)rect_points[1]);
    float height = radialDistance((Point)rect_points[1], (Point)rect_points[2]);

    if (width > height)
        elongation = width / height;
    else
        elongation = height / width;
}

inline cv::Point mms::Segment_Class::focus_point()
{
    int x_sum = 0, y_sum = 0;
    for (auto point : conture) {
        x_sum += point.x;
        y_sum += point.y;
    }
    return cv::Point(x_sum/conture.size(), y_sum/conture.size());
}

inline float mms::Segment_Class::avg_contureDistance()
{
    vector<float> radial_dists;
    Point focus = focus_point();

    for (Point point : conture) {
        radial_dists.push_back(radialDistance(point, focus));
    }

    double sum_dists = 0;

    for (float dist : radial_dists) {
        sum_dists += dist;
    }

    return (sum_dists / (float)radial_dists.size()) / 10.0;
}

inline float mms::Segment_Class::avg_radialDistance()
{
    vector<float> radial_dists;
    vector<float> norm_radial_dists;
    Point focus = focus_point();

    for (Point point : conture) {
        radial_dists.push_back(radialDistance(point, focus));
    }
    norm_radial_dists = normalized_radialDistances(radial_dists);

    double sum_dists = 0;

    for (float dist : norm_radial_dists) {
        sum_dists += dist;
    }

    return sum_dists/(float)norm_radial_dists.size();
}

inline float mms::Segment_Class::radialDistance(cv::Point point, cv::Point focus_point)
{
    return sqrt(pow(point.x - focus_point.x, 2) + pow(point.y - focus_point.y, 2));
}

inline vector<float> mms::Segment_Class::normalized_radialDistances(std::vector<float> radial_distances)
{
    float max_dist = 0;
    for (float dist : radial_distances) {
        if (dist > max_dist) {
            max_dist = dist;
        }
    }
    vector<float> norm_radial_distances;
    for (float dist : radial_distances) {
        norm_radial_distances.push_back(dist / max_dist);
    }
    return norm_radial_distances;
}



inline std::vector<double> mms::Segment_Class::normalized_conture_moment()
{
    Point s = focus_point();
    double m1 = 0;
    std::vector<double> normalized_conture_moments;

    for (auto point : conture) {
        double d = sqrt(pow(point.x - s.x, 2) + pow(point.y - s.y, 2));
        m1 += d;
    }

    m1 *= (1.0 / (float)conture.size());

    for (int k = 0; k < conture.size(); k++) {
        double mk = 0;
        for (auto point : conture) {
            double d = sqrt(pow(point.x - s.x, 2) + pow(point.y - s.y, 2));
            mk += pow(d - m1, k);
        }
        mk *= (1.0 / (float)conture.size());
        normalized_conture_moments.push_back(mk / sqrt(mk));
    }

    return normalized_conture_moments;
}

inline cv::Vec3f mms::Segment_Class::rgb_to_hsv(cv::Vec3f rgb)
{
    Vec3f hsv;
    float _r = rgb[0] / 255;
    float _g = rgb[1] / 255;
    float _b = rgb[2] / 255;

    float cMax = max(_r, _g);
    cMax = max(cMax, _b);
    float cMin = min(_r, _g);
    cMin = min(cMin, _b);
    float delta = cMax - cMin;

    //Hue calculation
    if (delta == 0) {
        hsv[0] = 0;
    }
    else if (cMax == _r) {
        hsv[0] = ((_g - _b) / delta);
    }
    else if (cMax == _g) {
        hsv[0] = 2 + ((_b - _r) / delta);
    }
    else if (cMax == _b) {
        hsv[0] = 4 + ((_r - _g) / delta);
    }
    hsv[0] *= 60;
    if (hsv[0] < 0) {
        hsv[0] += 360;
    }

    //Saturation calculation
    if (cMax != 0) {
        hsv[1] = delta / cMax;
    }
    else {
        hsv[1] = 0;
    }

    //Value calculation
    hsv[2] = cMax;

    return hsv;
}