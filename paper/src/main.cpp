#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    // Read the input image
    Mat image = imread("input_image.jpg");

    if (image.empty()) {
        cout << "Could not open or find the image!" << endl;
        return -1;
    }

    // Convert the image to grayscale
    Mat gray;
    cvtColor(image, gray, COLOR_BGR2GRAY);

    // Threshold the grayscale image to get a binary image
    Mat binary;
    threshold(gray, binary, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);

    // Perform morphological operations to clean up the binary image
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    morphologyEx(binary, binary, MORPH_OPEN, kernel, Point(-1, -1), 2);
    morphologyEx(binary, binary, MORPH_CLOSE, kernel, Point(-1, -1), 2);

    // Perform distance transform to get the distance map
    Mat dist;
    distanceTransform(binary, dist, DIST_L2, 5);

    // Normalize the distance map
    normalize(dist, dist, 0, 1, NORM_MINMAX);

    // Threshold the distance map to obtain markers
    threshold(dist, dist, 0.4, 1, THRESH_BINARY);

    // Convert the marker image to 8-bit
    Mat markers;
    dist.convertTo(markers, CV_8U);

    // Apply watershed algorithm
    watershed(image, markers);

    // Generate random colors for segmentation visualization
    vector<Vec3b> colors;
    for (int i = 0; i < 100; i++) {
        int b = theRNG().uniform(0, 256);
        int g = theRNG().uniform(0, 256);
        int r = theRNG().uniform(0, 256);
        colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
    }

    // Create segmentation result
    Mat segmentationResult = Mat::zeros(markers.size(), CV_8UC3);
    for (int i = 0; i < markers.rows; i++) {
        for (int j = 0; j < markers.cols; j++) {
            int index = markers.at<int>(i, j);
            if (index > 0 && index <= 100) {
                segmentationResult.at<Vec3b>(i, j) = colors[index - 1];
            } else {
                segmentationResult.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
            }
        }
    }

    // Display the result
    imshow("Segmentation Result", segmentationResult);
    waitKey(0);

    return 0;
}
