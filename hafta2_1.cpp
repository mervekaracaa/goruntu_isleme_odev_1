#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {
    // Fotoğraf yolunu bash ortamından alıyoruz
    string img_path = string(getenv("FOTOGRAF_YOLU"));
    
    // 1. Resmi TEK KANALLI (Grayscale) olarak oku
    Mat img = imread(img_path, IMREAD_GRAYSCALE);
    if (img.empty()) {
        cout << "HATA: Fotograf okunamadi! Yol yanlis olabilir: " << img_path << endl;
        return -1;
    }

    // 2. 8-Bit -> 6-Bit Parlaklık Nicemleme (Quantization)
    // 8-bit (256 seviye) değerlerini 6-bit (64 seviye) aralığına düşürüyoruz.
    // Bunun en pratik yolu her pikseli 4'e bölüp (küsuratı atıp) tekrar 4 ile çarpmaktır.
    Mat quantized_img = img.clone();
    for (int r = 0; r < img.rows; r++) {
        for (int c = 0; c < img.cols; c++) {
            uchar pixel = img.at<uchar>(r, c);
            quantized_img.at<uchar>(r, c) = (pixel / 4) * 4; 
        }
    }

    // 3. Sonucu yeniboyut_resimler klasörüne kaydet
    string output_name = "/content/yeniboyut_resimler/gorev1_6bit.jpg";
    imwrite(output_name, quantized_img);
    
    cout << "[BASARILI] Goruntu tek kanalli okundu, 6-bit'e nicemlendi ve '" << output_name << "' olarak kaydedildi!" << endl;
    
    return 0;
}
