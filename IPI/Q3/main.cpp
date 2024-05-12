#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;

void performDFT(const Mat& input, Mat& output);
void performIDFT(const Mat& input, Mat& output);
void showMagnitudeSpectrum(const Mat& dftImage);
void high_Butterworth(const Mat image, Mat& output, int n, int D0, const std::vector<Point>& points);
void autoContrast(cv::Mat& img);

int main(int argc, char *argv[]) {
    // Checking if the program arguments passage is done right
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <image_path>" << std::endl;
        return -1;
    }

    // Get the image file path from the command-line argument
    std::string imagePath = argv[1];

    // Loading the image
    Mat image = imread(imagePath, IMREAD_GRAYSCALE);
    if (image.empty()) {
        std::cerr << "Failed to load the image!" << std::endl;
        return -1;
    }
    int height = image.cols ;
    int width = image.rows;


    cv::namedWindow("Original Image", cv::WINDOW_NORMAL);
    cv::resizeWindow("Original Image", height, width);
    imshow("Original Image", image);


    // Add multiple points as needed
    std::vector<Point> points =  {Point(39,30),Point(-39,30),Point(78,30),Point(-78,30)};
    std::vector<int> D0_value = {10,10,5,5};

    Mat ButterResult;
    high_Butterworth(image, ButterResult, 4, 10, points);

    autoContrast(ButterResult);
    //printing the result of the butterResult
    cv::namedWindow("Butterworth", cv::WINDOW_NORMAL);
    cv::resizeWindow("Butterworth", height, width);
    imshow("Butterworth", ButterResult);
    

    // To close the window press "ESC"
    while (true) {
        int key = waitKey(0);
        if (key == 27) 
            destroyAllWindows();
        }

    return 0;
}



void performDFT(const Mat& input, Mat& output) {
    // Convert to floating point image
    Mat img;
    input.convertTo(img, CV_32F);

    // Perform DFT
    Mat dftImage;
    dft(img, dftImage, DFT_COMPLEX_OUTPUT);

    // Shift DFT
    int cx = dftImage.cols / 2;
    int cy = dftImage.rows / 2;

    Mat q0(dftImage, Rect(0, 0, cx, cy));
    Mat q1(dftImage, Rect(cx, 0, cx, cy));
    Mat q2(dftImage, Rect(0, cy, cx, cy));
    Mat q3(dftImage, Rect(cx, cy, cx, cy));

    Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);

    output = dftImage.clone();
}

void performIDFT(const Mat& input, Mat& output) {
    // Perform IDFT
    Mat idftImage;
    idft(input, idftImage, DFT_SCALE | DFT_REAL_OUTPUT);

    // Shift back IDFT
    int cx = idftImage.cols / 2;
    int cy = idftImage.rows / 2;

    Mat q0(idftImage, Rect(0, 0, cx, cy));
    Mat q1(idftImage, Rect(cx, 0, cx, cy));
    Mat q2(idftImage, Rect(0, cy, cx, cy));
    Mat q3(idftImage, Rect(cx, cy, cx, cy));

    Mat tmp;
    q2.copyTo(tmp);
    q3.copyTo(q2);
    tmp.copyTo(q3);

    q0.copyTo(tmp);
    q1.copyTo(q0);
    tmp.copyTo(q1);

    q1.copyTo(tmp);
    q0.copyTo(q1);
    tmp.copyTo(q0);

    q2.copyTo(tmp);
    q3.copyTo(q2);
    tmp.copyTo(q3);


    // Convert to unsigned char
    idftImage.convertTo(output, CV_8U);
}

void showMagnitudeSpectrum(const Mat& dftImage) {
    Mat planes[2];
    split(dftImage, planes);
    magnitude(planes[0], planes[1], planes[0]);
    Mat mag = planes[0];

    mag += Scalar::all(1);
    log(mag, mag);

    normalize(mag, mag, 0, 1, NORM_MINMAX);
    imshow("Magnitude Spectrum", mag);
    waitKey(0);
}

void high_Butterworth(const Mat image, Mat& output, int n, int D0, const std::vector<Point>& points) {
    Mat input;
    performDFT(image, input);

    // The origin
    int cx = input.cols / 2;
    int cy = input.rows / 2;

    // Compute distances for all points
    Mat distances(input.size(), CV_32FC2);
    for (int i = 0; i < input.rows; ++i) {
        for (int j = 0; j < input.cols; ++j) {
            float minDistance = std::numeric_limits<float>::max();
            for (const auto& point : points) {
                // Corrected distance calculation
                int dx = j - cx - point.y; // Distance from center in x direction
                int dy = i - cy - point.x; // Distance from center in y direction
                float distance = sqrt(dx*dx + dy*dy);
                if (distance < minDistance) {
                    minDistance = distance;
                }
            }
            distances.at<float>(i, j) = minDistance;
        }
    }

    // Compute the Butterworth high-pass filter for each point
    Mat butterworthFilter(input.size(), CV_32FC2);
    for (int i = 0; i < input.rows; ++i) {
        for (int j = 0; j < input.cols; ++j) {
            butterworthFilter.at<float>(i, j) =  1.0 / (1 + pow(D0 / distances.at<float>(i, j), 2 * n));
        }
    }

    Mat complexInput;
    input.convertTo(complexInput, CV_32FC2);

    Mat filteredImage(input.size(), CV_32FC2);
    mulSpectrums(complexInput,  butterworthFilter, filteredImage, 0);

    performIDFT(filteredImage,output);
    
}

void autoContrast(cv::Mat& img) {
    // histogram
    cv::Mat hist;
    float range[] = {0, 256};
    const float* histRange = {range};
    int histSize = 256;
    calcHist(&img, 1, nullptr, cv::Mat(), hist, 1, &histSize, &histRange);

    // Find minimum and maximum values of pixel intensity
    int minIntensity = 0, maxIntensity = 255;
    for (int i = 0; i < histSize; ++i) {
        if (hist.at<float>(i) > 0) {
            minIntensity = i;
            break;
        }
    }
    for (int i = histSize - 1; i >= 0; --i) {
        if (hist.at<float>(i) > 0) {
            maxIntensity = i;
            break;
        }
    }

    //contrast stretching = auto contrast
    img = (img - minIntensity) * (255.0 / (maxIntensity - minIntensity));
    img.setTo(0, img < 0);
    img.setTo(255, img > 255);
    img.convertTo(img, CV_8U);

    //histogram equalization = auto tone 
    double alpha = 255.0 / (maxIntensity - minIntensity);
    img = img * alpha + minIntensity - minIntensity * alpha;
}