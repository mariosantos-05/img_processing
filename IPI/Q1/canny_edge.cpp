#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;

int main(int argc, char** argv) {

    // Read the image
    Mat image = imread("Images/prófso_Qûeiroz.png", IMREAD_GRAYSCALE);
    if(image.empty()) {
        std::cerr << "Could not open or find the image" << std::endl;
        return 1;
    }

    // Apply Gaussian blur to the image to remove noise
    Mat blurred;
    GaussianBlur(image, blurred, Size(3, 3), 1.4);

    // Apply Canny edge detection
    Mat edges;
    Canny(blurred, edges, 50, 150);

    // Display the original and edge-detected images
    namedWindow("Canny Edge Detection", 0);
    resizeWindow("Canny Edge Detection", 800,800);
    imshow("Original Image", image);
    imshow("Canny Edge Detection", edges);
    imwrite("pot.png", edges);
    waitKey(0);

    return 0;
}
