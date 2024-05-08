#include <opencv2/opencv.hpp>

using namespace cv;

int main() {
    // Read the image
    Mat image = imread("Images/alex.png", IMREAD_GRAYSCALE);
    
    if (image.empty()) {
        std::cerr << "Could not open or find the image!\n";
        return -1;
    }

    // Convert the image to float32
    Mat floatImage;
    image.convertTo(floatImage, CV_32F);

    // Apply the Laplacian filter to obtain the high-pass filtered image
    Mat highPassImage;
    Laplacian(floatImage, highPassImage, CV_32F);

    // Convert the result back to uint8 format
    Mat highPassImageUint8;
    highPassImage.convertTo(highPassImageUint8, CV_8U);

    // Display the original and high-pass filtered images
    namedWindow("High Pass Filtered Image", 0);
    resizeWindow("High Pass Filtered Image", 800,800);
    imshow("Original Image", image);
    imshow("High Pass Filtered Image", highPassImageUint8);
    waitKey(0);

    return 0;
}
