#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>

struct FrameYUV {
    std::vector<unsigned char> Y; // Componente Y
    std::vector<unsigned char> U; // Componente U
    std::vector<unsigned char> V; // Componente V
    int tamanho_y; // Tamanho da componente Y
    int tamanho_u; // Tamanho da componente U
    int tamanho_v; // Tamanho da componente V
};

class YUVPlayer {
private:
    std::ifstream arquivo;
    int largura;
    int altura;

public:
    YUVPlayer(const std::string& nome_arquivo, int largura, int altura) : largura(largura), altura(altura) {
        arquivo.open(nome_arquivo, std::ios::binary);
        if (!arquivo.is_open()) {
            std::cerr << "Erro ao abrir o arquivo YUV." << std::endl;
            exit(1);
        }
    }

    ~YUVPlayer() {
        arquivo.close();
    }

    FrameYUV lerQuadroYUV() {
        FrameYUV frame;
        frame.tamanho_y = largura * altura;
        frame.tamanho_u = (largura / 2) * (altura / 2);
        frame.tamanho_v = (largura / 2) * (altura / 2);

        frame.Y.resize(frame.tamanho_y);
        frame.U.resize(frame.tamanho_u);
        frame.V.resize(frame.tamanho_v);

        arquivo.read(reinterpret_cast<char*>(&frame.Y[0]), frame.tamanho_y);
        arquivo.read(reinterpret_cast<char*>(&frame.U[0]), frame.tamanho_u);
        arquivo.read(reinterpret_cast<char*>(&frame.V[0]), frame.tamanho_v);

        return frame;
    }

    void reproduzirVideo() {
        cv::namedWindow("Imagem RGB");
        while (true) {
            FrameYUV quadro = lerQuadroYUV();
            if (arquivo.eof())
                break;

            cv::Mat matYUV(altura + altura / 2, largura, CV_8UC1);
            memcpy(matYUV.data, quadro.Y.data(), quadro.tamanho_y);
            memcpy(matYUV.data + quadro.tamanho_y, quadro.U.data(), quadro.tamanho_u);
            memcpy(matYUV.data + quadro.tamanho_y + quadro.tamanho_u, quadro.V.data(), quadro.tamanho_v);

            cv::resize(matYUV, matYUV, cv::Size(largura, altura));

            cv::Mat matRGB;
            cv::cvtColor(matYUV, matRGB, cv::COLOR_YUV2RGB);

            cv::imshow("Imagem RGB", matRGB);

            if (cv::waitKey(30) == 'q')
                break;
        }
        cv::destroyAllWindows();
    }
};

int main() {
    std::string nome_arquivo = "foreman.yuv";
    int largura = 352;
    int altura = 288;

    YUVPlayer player(nome_arquivo, largura, altura);
    player.reproduzirVideo();

    return 0;
}
