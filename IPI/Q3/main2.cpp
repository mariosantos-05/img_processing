#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat createButterworthHPF(Size size, double cutoffFreq, int order) {
    Mat hpf(size, CV_32F);

    Point center(size.width / 2, size.height / 2);
    double radius;

    for (int i = 0; i < size.height; ++i) {
        for (int j = 0; j < size.width; ++j) {
            radius = sqrt(pow(i - center.y, 2) + pow(j - center.x, 2));
            hpf.at<float>(i, j) = 1 / (1 + pow(cutoffFreq / radius, 2 * order));
        }
    }

    return hpf;
}

void fftshift(Mat& mag) {
    int cx = mag.cols / 2;
    int cy = mag.rows / 2;

    Mat tmp;
    Mat q0(mag, Rect(0, 0, cx, cy));
    Mat q1(mag, Rect(cx, 0, cx, cy));
    Mat q2(mag, Rect(0, cy, cx, cy));
    Mat q3(mag, Rect(cx, cy, cx, cy));

    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);
}

int main() {
    Mat image = imread("moire.tif", IMREAD_GRAYSCALE);
    if (image.empty()) {
        cout << "Não foi possível carregar a imagem!" << endl;
        return -1;
    }

    Size size = image.size();
    double cutoffFreq = 0.1;
    int order = 2;

    Mat hpf = createButterworthHPF(size, cutoffFreq, order);

    Mat planes[] = {Mat_<float>(image), Mat::zeros(image.size(), CV_32F)};
    Mat complexImage;
    merge(planes, 2, complexImage);

    dft(complexImage, complexImage);

    fftshift(complexImage);

    Mat filteredImage;
    mulSpectrums(complexImage, hpf, filteredImage, 0);

    fftshift(filteredImage);

    Mat result;
    idft(filteredImage, result, DFT_SCALE | DFT_REAL_OUTPUT);

    result.convertTo(result, CV_8U);

    imshow("Original", image);
    imshow("Filtrada", result);
    waitKey(0);

    return 0;
}
