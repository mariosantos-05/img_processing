#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

int main() {
    // Read the image
    Mat image = imread("../img/queiroz.png");

    if (image.empty()) {
        cout << "Could not open or find the image." << endl;
        return -1;
    }

    // Reshape the image to a 2D matrix of pixels
    Mat reshaped_image = image.reshape(1, image.rows * image.cols);
    reshaped_image.convertTo(reshaped_image, CV_32F); // Convert to float for k-means

    // Define the number of clusters (segments)
    int K = 4;

    // Define criteria and apply k-means
    TermCriteria criteria(TermCriteria::EPS + TermCriteria::COUNT, 10, 1.0);
    Mat labels, centers;
    kmeans(reshaped_image, K, labels, criteria, 3, KMEANS_PP_CENTERS, centers);

    // Reshape the segmented image
    Mat segmented_image = Mat::zeros(image.size(), image.type());
    int index = 0;
    for (int y = 0; y < image.rows; y++) {
        for (int x = 0; x < image.cols; x++) {
            int cluster_label = labels.at<int>(index);
            segmented_image.at<Vec3b>(y, x) = centers.at<Vec3f>(cluster_label, 0);
            index++;
        }
    }

    // Display the original image
    namedWindow("Original Image", WINDOW_NORMAL);
    imshow("Original Image", image);

    // Display each color cluster in separate windows
    for (int i = 0; i < K; ++i) {
        Mat color_cluster = Mat::zeros(image.size(), image.type());
        for (int y = 0; y < image.rows; y++) {
            for (int x = 0; x < image.cols; x++) {
                if (labels.at<int>(y * image.cols + x) == i) {
                    color_cluster.at<Vec3b>(y, x) = segmented_image.at<Vec3b>(y, x);
                }
            }
        }
        string window_name = "Color Cluster " + to_string(i);
        namedWindow(window_name, WINDOW_NORMAL);
        imshow(window_name, color_cluster);
    }

    waitKey(0);
    return 0;
}
