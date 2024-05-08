#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

using namespace cv;

// Função para calcular o filtro rejeita-notch
Mat rejeitaNotchFilter(Mat input, double D0, int uk, int vk) {
    Mat output = input.clone();

    int rows = input.rows;
    int cols = input.cols;

    // Converter a imagem para ponto flutuante de precisão única
    input.convertTo(input, CV_32F);

    // Criar o filtro passa-alta Butterworth
    Mat butterworthHPF(rows, cols, CV_32F); // Alteração aqui
    double center_u = cols / 2;
    double center_v = rows / 2;

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            double distance = sqrt(pow(x - center_u - uk, 2) + pow(y - center_v - vk, 2));
            butterworthHPF.at<float>(y, x) = 1.0 / (1 + pow(D0 / distance, 4)); // Alteração aqui
        }
    }

    // Aplicar o filtro rejeita-notch
    Mat padded;
    copyMakeBorder(input, padded, 0, rows, 0, cols, BORDER_CONSTANT, Scalar::all(0));

    dft(padded, padded, DFT_COMPLEX_OUTPUT);
    Mat planes[] = {Mat::zeros(padded.size(), CV_32F), Mat::zeros(padded.size(), CV_32F)}; // Alteração aqui
    split(padded, planes);

    Mat complexI;
    merge(planes, 2, complexI);

    // Converter a matriz de filtro para o mesmo tipo que a imagem
    butterworthHPF.convertTo(butterworthHPF, CV_32F); // Alteração aqui

    mulSpectrums(complexI, butterworthHPF, complexI, 0);

    idft(complexI, complexI);

    split(complexI, planes);
    magnitude(planes[0], planes[1], output);
    normalize(output, output, 0, 255, NORM_MINMAX);

    output = output(Rect(0, 0, cols, rows));

    return output;
}

int main() {
    // Carregar a imagem (substitua esta parte com a leitura do arquivo "MOIRE.TIF")
    Mat image = imread("moire.tif", IMREAD_GRAYSCALE);
    if (image.empty()) {
        std::cerr << "Erro ao carregar a imagem." << std::endl;
        return -1;
    }

    // 3.1 Exibir a imagem original
    imshow("Imagem Original", image);

    // 3.2 Aplicar o filtro rejeita-notch para cada par especificado
    Mat filtered_image = image.clone();

    filtered_image = rejeitaNotchFilter(filtered_image, 10, 39, 30);
    filtered_image = rejeitaNotchFilter(filtered_image, 10, -39, 30);
    filtered_image = rejeitaNotchFilter(filtered_image, 5, 78, 30);
    filtered_image = rejeitaNotchFilter(filtered_image, 5, -78, 30);

    // Exibir a imagem filtrada
    imshow("Imagem Filtrada", filtered_image);
    waitKey(0);

    return 0;
}
