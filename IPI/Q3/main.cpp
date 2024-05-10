#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;

void dftFunction(const Mat& input, Mat& output) {
    // Convert input to floating point type
    Mat floatInput;
    input.convertTo(floatInput, CV_32F);

    // Adding padding to the image (if it's not a power of 2)
    Mat padded; 
    int m = getOptimalDFTSize(floatInput.rows);
    int n = getOptimalDFTSize(floatInput.cols);
    copyMakeBorder(floatInput, padded, 0, m - floatInput.rows, 0, n - floatInput.cols, BORDER_CONSTANT, Scalar::all(0));

    Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) }; 
    Mat complexImage;
    merge(planes, 2, complexImage); 

    dft(complexImage, complexImage); 

    // Compute the magnitude and switch to logarithmic scale
    split(complexImage, planes); 
    magnitude(planes[0], planes[1], planes[0]); 
    Mat magnitudeImage = planes[0];

    magnitudeImage += Scalar::all(1); // Switch to logarithmic scale
    log(magnitudeImage, magnitudeImage);

    // Crop the spectrum if it has an odd number of rows or columns
    magnitudeImage = magnitudeImage(Rect(0, 0, magnitudeImage.cols & -2, magnitudeImage.rows & -2));

    // Rearrange the quadrants of the Fourier image
    int cx = magnitudeImage.cols / 2;
    int cy = magnitudeImage.rows / 2;

    // Create an ROI-per-quadrant
    Mat q0(magnitudeImage, Rect(0, 0, cx, cy));  
    Mat q1(magnitudeImage, Rect(cx, 0, cx, cy));
    Mat q2(magnitudeImage, Rect(0, cy, cx, cy)); 
    Mat q3(magnitudeImage, Rect(cx, cy, cx, cy)); 

    // Swapping the quadrants
    Mat tmp; 
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);
    q1.copyTo(tmp); 
    q2.copyTo(q1);
    tmp.copyTo(q2);

    // Convert magnitudeImage to output
    normalize(magnitudeImage, output, 0, 1, NORM_MINMAX); 
}

void high_Butterworth(const Mat image, Mat& output, int n, int D0, const std::vector<Point>& points) {
    Mat input;
    dftFunction(image, input);

    // The origin
    int cx = input.cols / 2;
    int cy = input.rows / 2;

    // Compute distances for all points
    Mat distances(input.size(), CV_32F);
    for (int i = 0; i < input.rows; ++i) {
        for (int j = 0; j < input.cols; ++j) {
            float minDistance = std::numeric_limits<float>::max();
            for (const auto& point : points) {
                // Compute the distance of each point from the specified center
                int dx = cx + point.y - j; // Distance from center in x direction
                int dy = cy - point.x - i; // Distance from center in y direction
                float distance = sqrt(dx*dx + dy*dy);
                if (distance < minDistance) {
                    minDistance = distance;
                }
            }
            distances.at<float>(i, j) = minDistance;
        }
    }

    // Compute the Butterworth high-pass filter for each point
    Mat butterworthFilter(input.size(), CV_32F);
    for (int i = 0; i < input.rows; ++i) {
        for (int j = 0; j < input.cols; ++j) {
            butterworthFilter.at<float>(i, j) += 1.0 / (1 + pow(sqrt((distances.at<float>(i, j) * distances.at<float>(i, j)) / D0 / D0), 2 * n));
        }
    }

    

    // Apply the filter to the input image in the frequency domain
    Mat filteredImage;
    multiply(input, butterworthFilter, filteredImage);
    imshow("filter", filteredImage);
    imshow("input", input);
    imshow("Butter", butterworthFilter);

    // Convert filteredImage to spatial domain
    Mat inverseTransform;
    idft(filteredImage, inverseTransform, DFT_INVERSE | DFT_SCALE);

    // Extract the real part (as the imaginary part should be close to zero)
    Mat planes[2];
    split(inverseTransform, planes);
    output = planes[0];

    // Normalize the output to [0, 1] range
    normalize(output, output, 0, 1, NORM_MINMAX);
}




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

    /* cv::namedWindow("Original", cv::WINDOW_NORMAL);
    cv::resizeWindow("Original", 360,360);
    imshow("Original", image);

    // Getting the Fourier Spectrum
    Mat dftResult;
    dftFunction(image, dftResult);
    //if you want to analyze the Fourier transformation done to the original image, uncomment the next line
    cv::namedWindow("Fourier Spectrum", cv::WINDOW_NORMAL);
    cv::resizeWindow("Fourier Spectrum", 360,360);
    imshow("Fourier Spectrum", dftResult);*/

    // Add multiple points as needed
    std::vector<Point> points =  {Point(39,30),Point(-39,30),Point(78,30),Point(-78,30)};
    std::vector<int> D0_value = {10,10,5,5};

    Mat ButterResult;
    high_Butterworth(image, ButterResult, 4, 10, points);




    
    //printing the result of the butterResult
    namedWindow("Butterworth", cv::WINDOW_NORMAL);
    resizeWindow("Butterworth", 360,360);
    imshow("Butterworth", ButterResult);
    

    // To close the window press "ESC"
    while (true) {
        int key = waitKey(0);
        if (key == 27) 
            destroyAllWindows();
        }

    return 0;
}
