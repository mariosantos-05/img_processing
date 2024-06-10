#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;

//TODO: make the D work individually for each point

// To COMPILE this code you have to run "make && ./Questão_03 <image_path>" inside this directory
// TIP: If a problem with core20 encountered try "unset GTK_PATH"


// Function to perform Discrete Fourier Transform (DFT) on input image
// Converts the input image to frequency domain
void performDFT(const Mat& input, Mat& output);

// Function to perform Inverse Discrete Fourier Transform (IDFT) on input frequency domain data
// Converts the frequency domain data back to spatial domain
void performIDFT(const Mat& input, Mat& output);

// Function to display the magnitude spectrum of the input Fourier transformed image
void showMagnitudeSpectrum(const Mat dftImage);


// Function to apply high-pass Butterworth filter to the input image at specified points
// n: Order of the Butterworth filter
// D0: Cutoff frequency
// points: Vector containing the coordinates of the points in the frequency domain
void high_Butterworth(const Mat image, Mat& output, int n, int D0, const std::vector<Point>& points);






int main(int argc, char *argv[]) {
    // Check if the correct number of command-line arguments is provided
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <image_path>" << std::endl;
        return -1; // Return error code if incorrect number of arguments
    }

    // Get the image file path from the command-line argument
    std::string imagePath = argv[1];

    // Load the image
    Mat image = imread(imagePath, IMREAD_GRAYSCALE);
    if (image.empty()) {
        std::cerr << "Failed to load the image!" << std::endl;
        return -1; // Return error code if image loading fails
    }



    // Get image dimensions
    int height = image.cols;
    int width = image.rows;

    // Define points for Butterworth filter
    std::vector<Point> points =  {Point(39,30),Point(-39,30),Point(78,30),Point(-78,30),Point(-39,-30),Point(39,-30),Point(-78,-30),Point(78,-30)};
    // Define cutoff frequencies for each point
    std::vector<int> D0_value = {10,10,5,5};

    // Apply Butterworth filter
    Mat ButterResult;
    high_Butterworth(image, ButterResult, 4, 10, points);

    // Display original and filtered images
    namedWindow("Original Image", WINDOW_NORMAL);
    resizeWindow("Original Image", height, width);
    imshow("Original Image", image);


    namedWindow("Butterworth", WINDOW_NORMAL);
    resizeWindow("Butterworth", height, width);
    
    imshow("Butterworth", ButterResult);

    // Save the filtered image (uncomment to save)
    imwrite("filterd.png", ButterResult);
    imwrite("Q3_Original.png", image);

    // Close the windows when ESC is pressed
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
    Mat idftImage;
    idft(input, idftImage, DFT_SCALE | DFT_REAL_OUTPUT);
    idftImage.convertTo(output, CV_8U);
}


void showMagnitudeSpectrum(const Mat dftImage) {
    Mat planes[2];
    split(dftImage, planes);
    magnitude(planes[0], planes[1], planes[0]);
    Mat mag = planes[0];

    mag += Scalar::all(1);
    log(mag, mag);
    int height = mag.cols;
    int width = mag.rows;
    normalize(mag, mag, 0, 1, NORM_MINMAX);
    namedWindow("input", WINDOW_NORMAL);
    resizeWindow("input", height, width);
    imshow("input", mag);
    imwrite("Queiroz.jpg", mag);
    waitKey(0);
}


void high_Butterworth(const Mat image, Mat& output, int n, int D0, const std::vector<Point>& points) {
    Mat input;
    performDFT(image, input);
    showMagnitudeSpectrum(input);

    // The origin of the image
    int cx = input.cols / 2;
    int cy = input.rows / 2;

    // Initialize Butterworth filter
    Mat butterworthFilter = Mat::ones(input.size(), CV_32FC2);
    
    // Apply the Butterworth filter for each specified point
    for (const auto& point : points) {
        Mat currentFilter = Mat::ones(input.size(), CV_32FC2);
        for (int i = 0; i < input.rows; ++i) {
            for (int j = 0; j < input.cols; ++j) {
                int dx = j - cx - point.y; // Distance from point in x direction
                int dy = i - cy - point.x; // Distance from point in y direction
                float distance = sqrt(dx*dx + dy*dy);
                currentFilter.at<float>(i, j) = 1.0 / (1.0 + pow(D0 / distance, 2 * n));
            }
        }
        butterworthFilter = butterworthFilter.mul(currentFilter);
    }

    // Convert the filter to a compatible type
    Mat planes[2] = { butterworthFilter, Mat::zeros(butterworthFilter.size(), CV_32F) };
    Mat complexFilter;
    merge(planes, 2, complexFilter);

    // Apply the filter to the input image in frequency domain
    Mat filteredImage;
    mulSpectrums(input, butterworthFilter, filteredImage, 1);


    // Perform IDFT to get the output image in spatial domain
    performIDFT(filteredImage, output);
}

