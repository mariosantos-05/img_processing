#include <opencv2/opencv.hpp>
using namespace cv;

// To COMPILE this code you have to run "make && ./Questão_02 <image_path>" inside this directory
// TIP: If a problem with core20 encountered try "unset GTK_PATH"

int main(int argc, char *argv[]) {
    
    // Checking if the program argumets passage is done right
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <image_path>" << std::endl;
        return -1;
    }

    // Get the image file path from the command-line argument
    std::string imagePath = argv[1];

    // Loading the image
    Mat image = imread(imagePath, IMREAD_GRAYSCALE);
    if (image.empty()) {
        std::cerr << "Fail to load the image!" << std::endl;
        return -1;
    }


    // 2.1 Straight Laplacian filter
    Mat sharpened_direct;
    Mat kernel_direct = (Mat_<float>(3,3) << 0, -1, 0, -1, 4, -1, 0, -1, 0); // Laplacian filtering 3x3
    filter2D(image, sharpened_direct, CV_8U, kernel_direct);

    // 2.2 Gaussian blur + Laplacian filter 
    Mat blurred;
    GaussianBlur(image, blurred, Size(3,3), 0.5); // Gaussian filter 3x3 with sigma = 0.5

    Mat sharpened_gaussian;
    Mat kernel_gaussian = (Mat_<float>(3,3) << 0, -1, 0, -1, 4, -1, 0, -1, 0); // Laplacian filtering 3x3
    filter2D(blurred, sharpened_gaussian, CV_8U, kernel_gaussian);

    // 2.3 Gaussian filter + Laplacian filter
    GaussianBlur(image, blurred, Size(3,3), 1.0); // Gaussian filter 3x3 with sigma = 1.0

    Mat sharpened_gaussian2;
    filter2D(blurred, sharpened_gaussian2, CV_8U, kernel_gaussian);

    // showing the results
    imshow("Sharpened Direct Laplacian", sharpened_direct);
    imshow("Sharpened Gaussian 0.5", sharpened_gaussian);
    imshow("Sharpened Gaussian 1.0", sharpened_gaussian2);


    // Saving the results in the directory (comment this section if you only testing the parameters)
    /*
    imwrite("Sharpened_Direct_Laplacian.png", sharpened_direct);
    imwrite("Sharpened_Gaussian_0.5.png", sharpened_gaussian);
    imwrite("Sharpened_Gaussian_1.0.png", sharpened_gaussian2);*/
    
    waitKey(0);

    return 0;
}



