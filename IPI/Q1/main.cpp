#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>
using namespace cv;

// To COMPILE this code you have to run "make && ./Questão_01" inside this directory
// TIP: If a problem with core20 encountered try "unset GTK_PATH"

// Function to read a single frame from a YUV file
void READ_YUV(const char* filename, int height, int width, int frame) {
    std::ifstream file(filename, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error opening YUV file." << std::endl;
        return;
    }

    // Calculate frame size in bytes
    int frameSize = height * width * 3 / 2; // 3 components: Y, U, and V

    // Position file pointer at the beginning of the desired frame
    file.seekg(frame * frameSize);

    // Allocate memory to store frame pixels
    std::vector<uchar> data(frameSize);

    // Read frame pixels
    file.read(reinterpret_cast<char*>(data.data()), frameSize);

    // Convert YUV data to BGR image
    Mat yuvImg(height * 3 / 2, width, CV_8UC1, data.data());
    Mat bgrImg;
    cvtColor(yuvImg, bgrImg, COLOR_YUV2BGR_I420);

    // Show the image
    imshow("YUV Frame", bgrImg);
    imwrite("YUV_FRAME.png", bgrImg);

    file.close();
}

// Function to upscale an image
void upscalling(){
    // Load the image
    Mat image = imread("YUV_FRAME.png",1);
    if (!image.empty()) {
        int rows = image.rows;
        int cols = image.cols;
        int height = rows * 2;
        int width = cols * 2;
        Mat black_image(height, width, CV_8UC3, Scalar(0,0,0));

        // Iterate through each pixel of black_image and insert every other pixel from the original image
        for (int y = 0; y < rows; y++) {
            for (int x = 0; x < cols; x++) {
                black_image.at<Vec3b>(y * 2, x * 2) = image.at<Vec3b>(y, x);
            }
        }

        //averaging the pixels in each row
        for(int y = 0; y < height-1; y++){
            for(int x = 1; x < width-1; x++){
                Vec3b sum = black_image.at<Vec3b>(y,x-1) + black_image.at<Vec3b>(y,x+1);
                Vec3b avr = sum/2; 
                black_image.at<Vec3b>(y,x) = avr;
            }
        }

        //averaging the pixels in each column
        for(int x = 0; x < width-1; x++){
            for(int y = 1; y < height-1; y++){
                Vec3b sum = black_image.at<Vec3b>(y-1,x) + black_image.at<Vec3b>(y+1,x);
                Vec3b avr = sum/2; 
                black_image.at<Vec3b>(y,x) = avr;
            }
            
        }

        //Apply a low-pass filter
        Mat finalImage;
        GaussianBlur(black_image, finalImage, Size(3, 3),0);
        
        //Show the final upscaled image
        namedWindow("Final Image", WINDOW_NORMAL);
        resizeWindow("Final Image", width, height);
        imshow("Final Image", finalImage);

    } else {
        std::cerr << "Error: Unable to load the image." << std::endl;
    }
}

// Function to upscale an image
void upscalling0(){
    // Load the image
    Mat image = imread("YUV_FRAME.png",1);
    if (!image.empty()) {
        int rows = image.rows;
        int cols = image.cols;
        int height = rows * 2;
        int width = cols * 2;
        Mat black_image(height, width, CV_8UC3, Scalar(0,0,0));

        // Iterate through each pixel of black_image and insert every other pixel from the original image
        for (int y = 0; y < rows; y++) {
            for (int x = 0; x < cols; x++) {
                black_image.at<Vec3b>(y * 2, x * 2) = image.at<Vec3b>(y, x);
            }
        }


        // copping the pixels in each column
        for(int x = 0; x < width-1; x++){
            for(int y = 1; y < height-1; y+=2){
                black_image.at<Vec3b>(y,x) = black_image.at<Vec3b>(y-1,x);
            }  
        }


        // copping the pixels in each row
        for(int y = 0; y < height-1; y++){
            for(int x = 1; x < width-1; x+=2){
                black_image.at<Vec3b>(y,x) = black_image.at<Vec3b>(y,x-1);
            }
        }

        //Apply a low-pass filter
        Mat finalImage;
        GaussianBlur(black_image, finalImage, Size(3, 3),0);
        
        //Show the final upscaled image
        namedWindow("Final Image(0)", WINDOW_NORMAL);
        resizeWindow("Final Image(0)", width, height);
        imshow("Final Image(0)", finalImage);

    } else {
        std::cerr << "Error: Unable to load the image." << std::endl;
    }
}

// Function to read a video file in YUV format
void READ_YUV_VID(const char* filename, int height, int width, int frameCount) {
    std::ifstream file(filename, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error opening YUV file." << std::endl;
        return;
    }

    // Calculate frame size in bytes
    int frameSize = height * width * 3 / 2; // 3 components: Y, U, and V

    // Allocate memory to store frame pixels
    std::vector<uchar> data(frameSize);

    // Process each frame
    for (int frame = 0; frame < frameCount; ++frame) {
        // Position file pointer at the beginning of the current frame
        file.seekg(frame * frameSize);

        // Read frame pixels
        file.read(reinterpret_cast<char*>(data.data()), frameSize);

        // Convert YUV data to BGR image
        Mat yuvImg(height * 3 / 2, width, CV_8UC1, data.data());
        Mat bgrImg;
        cvtColor(yuvImg, bgrImg, COLOR_YUV2BGR_I420);

        // Show the image
        imshow("YUV Frame", bgrImg);
        waitKey(1); // Add a short delay to display the frame (30 ms)

        // Save the frame as an image file (not currently used in this function)
        std::string filename = "YUV_FRAME_" + std::to_string(frame) + ".png";
    }

    file.close();
}

int main() {
    const char* filename = "foreman.yuv"; // YUV file name
    int height = 288; // Height of the video frames
    int width = 352; // Width of the video frames
    int frame = 10; // Desired frame to read

    // Call to read a single frame from the YUV file
    READ_YUV(filename, height, width, frame);

    // Call to read and display a sequence of frames from the YUV file
    //READ_YUV_VID(filename,height, width, 1000);

    // Call to upscale the frame and display the result
    upscalling0();

    upscalling();

    // Close the windows when ESC is pressed
    while (true) {
        int key = waitKey(0);
        if (key == 27) // ASCII value of ESC key
            destroyAllWindows();
    }

    return 0;
}
