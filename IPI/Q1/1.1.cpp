#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>

// Estrutura para armazenar os dados de um quadro YUV
struct FrameYUV {
    std::vector<unsigned char> Y; // Componente Y
    std::vector<unsigned char> U; // Componente U
    std::vector<unsigned char> V; // Componente V
};

// Função para ler um quadro YUV do arquivo
FrameYUV lerQuadroYUV(std::ifstream& arquivo, int largura, int altura) {
    FrameYUV frame;

    // Calcula o tamanho em bytes de cada componente
    int tamanho_y = largura * altura;
    int tamanho_u = (largura / 2) * (altura / 2);
    int tamanho_v = (largura / 2) * (altura / 2);

    // Aloca espaço para os pixels Y, U e V
    frame.Y.resize(tamanho_y);
    frame.U.resize(tamanho_u);
    frame.V.resize(tamanho_v);

    // Lê os pixels Y
    arquivo.read(reinterpret_cast<char*>(&frame.Y[0]), tamanho_y);

    // Lê os pixels U
    arquivo.read(reinterpret_cast<char*>(&frame.U[0]), tamanho_u);

    // Lê os pixels V
    arquivo.read(reinterpret_cast<char*>(&frame.V[0]), tamanho_v);

    return frame;
}

int main() {
    // Abre o arquivo YUV
    std::ifstream arquivo("arquivo.yuv", std::ios::binary);
    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir o arquivo YUV." << std::endl;
        return 1;
    }

    // Define as dimensões do vídeo
    int largura = 352; // exemplo
    int altura = 288;  // exemplo

    // Crie uma janela para exibir a imagem
    cv::namedWindow("Imagem RGB");

    // Lê os quadros do arquivo YUV
    while (true) {
        // Lê um quadro do arquivo
        FrameYUV quadro = lerQuadroYUV(arquivo, largura, altura);

        // Verifica se chegou ao fim do arquivo
        if (arquivo.eof())
            break;

        // Converte os dados YUV para o formato OpenCV Mat
        cv::Mat matYUV(altura + altura / 2, largura, CV_8UC1);
        memcpy(matYUV.data, quadro.Y.data(), quadro.Y.size());
        memcpy(matYUV.data + tamanho_y, quadro.U.data(), quadro.U.size());
        memcpy(matYUV.data + tamanho_y + tamanho_u, quadro.V.data(), quadro.V.size());

        // Converte a imagem para RGB
        cv::Mat matRGB;
        cv::cvtColor(matYUV, matRGB, cv::COLOR_YUV2RGB);

        // Exibe a imagem na janela
        cv::imshow("Imagem RGB", matRGB);

        // Aguarda um tempo e verifica se foi pressionada a tecla 'q' para sair
        if (cv::waitKey(30) == 'q')
            break;
    }

    // Fechar o arquivo
    arquivo.close();

    // Fecha a janela ao final
    cv::destroyAllWindows();

    return 0;
}
