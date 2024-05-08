#include <opencv2/opencv.hpp>
#include <iostream>

//export PATH="/usr/bin:$PATH"

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

//first question 
void upscalling(){
    cv::Mat image = cv::imread("../IPI/Images/prófso_Qûeiroz.png", cv::IMREAD_GRAYSCALE);
    if (!image.empty()) {
        int rows = image.rows;
        int cols = image.cols;
        int height = rows * 2;
        int width = cols * 2;
        cv::Mat black_image(height, width, CV_8UC1, cv::Scalar(0));

        // Iterate through each pixel of black_image and insert every other pixel from the original image
        for (int y = 0; y < rows; y++) {
            for (int x = 0; x < cols; x++) {
                black_image.at<uchar>(y * 2, x * 2) = image.at<uchar>(y, x);
            }
        }

        //avarage of the pixels in the row
        for(int y = 0; y < height-1; y++){
            for(int x = 1; x < width-1; x++){
                int sum = black_image.at<uchar>(y,x-1) + black_image.at<uchar>(y,x+1);
                int avr = sum/2; 
                black_image.at<uchar>(y,x) = avr;
            }
        }

        //avarage of the pixels in the colum
        for(int x = 0; x < width-1; x++){
            for(int y = 1; y < height-1; y++){
                int sum = black_image.at<uchar>(y-1,x) + black_image.at<uchar>(y+1,x);
                int avr = sum/2; 
                black_image.at<uchar>(y,x) = avr;
            }
            
        }


        cv::Mat equalized_image;
        cv::equalizeHist(black_image, equalized_image);


        //Aply the low-pass filter
        cv::Mat blurredImage;
        cv::GaussianBlur(equalized_image, blurredImage, cv::Size(5, 5),0);


        //showing the final uspaceled image
        cv::namedWindow("Final Image", cv::WINDOW_NORMAL);
        cv::resizeWindow("Final Image", width, height);
        
        //The original Image of input
        cv::namedWindow("Original Image", cv::WINDOW_NORMAL);
        cv::resizeWindow("Original Image", rows, cols);
        cv::imshow("Original Image", image);
        cv::imshow("Final Image", blurredImage);
        cv::imwrite("Du_mundu.jpg", blurredImage);

        //The adjusted Image
        autoContrast(blurredImage);
        cv::namedWindow("Auto Adjusted", cv::WINDOW_NORMAL);
        cv::resizeWindow("Auto Adjusted", width, height);
        cv::imshow("Auto Adjusted", blurredImage);
        while (true) {
        int key = cv::waitKey(0);
        if (key == 27) 
            break;
        }
        cv::destroyAllWindows();
        
    } else {
        std::cout << "Error: Unable to load the image." << std::endl;
    }
    
}

int main() {
    upscalling();
    return 0;
    
}
