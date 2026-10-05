#include <iostream>
#include <vector>
#include <cmath>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {
    string img_path = string(getenv("FOTOGRAF_YOLU"));
    Mat img = imread(img_path, IMREAD_GRAYSCALE);
    if (img.empty()) {
        cout << "HATA: Fotograf okunamadi! Lutfen yolu kontrol et." << endl;
        return -1;
    }
    
    cout << "=== 3. HAFTA: HISTOGRAM ESITLEME (HAZIR FONKSIYONSUZ) ===" << endl;

    // 1. ADIM: 256-değerli Histogram Çıkarma
    int hist[256] = {0};
    int total_pixels = img.rows * img.cols;
    
    for (int r = 0; r < img.rows; r++) {
        for (int c = 0; c < img.cols; c++) {
            hist[img.at<uchar>(r, c)]++;
        }
    }
    cout << "[1/4] Piksel histogrami manuel olarak hesaplandi." << endl;

    // 2. ADIM: Kümülatif Dağılım Değerlerini (CDF) Hesaplama
    int cdf[256] = {0};
    cdf[0] = hist[0];
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i - 1] + hist[i];
    }
    cout << "[2/4] CDF (Kümülatif Dağılım Fonksiyonu) hesaplandi." << endl;

    // Eşitleme formülü için sıfırdan büyük en küçük CDF değerini buluyoruz
    int cdf_min = 0;
    for (int i = 0; i < 256; i++) {
        if (cdf[i] > 0) {
            cdf_min = cdf[i];
            break;
        }
    }

    // 3. ADIM: Histogram Eşitleme Haritasını Oluşturma
    // Formül: h(v) = round( (cdf[v] - cdf_min) / (Toplam_Piksel - cdf_min) * 255 )
    int mapping[256] = {0};
    for (int i = 0; i < 256; i++) {
        float val = (float)(cdf[i] - cdf_min) / (total_pixels - cdf_min) * 255.0f;
        if (val < 0) val = 0;
        if (val > 255) val = 255;
        mapping[i] = round(val);
    }
    cout << "[3/4] Matematiksel esitleme (equalization) haritasi olusturuldu." << endl;

    // 4. ADIM: Yeni Görüntüyü Üretme
    Mat eq_img = img.clone();
    for (int r = 0; r < img.rows; r++) {
        for (int c = 0; c < img.cols; c++) {
            uchar old_pixel = img.at<uchar>(r, c);
            eq_img.at<uchar>(r, c) = saturate_cast<uchar>(mapping[old_pixel]);
        }
    }
    cout << "[4/4] Yeni piksel degerleri goruntuye islendi." << endl;

    // Çıktı resmini kaydetme
    string output_path = "/content/Hafta_3/histogram_esitlenmis.jpg";
    imwrite(output_path, eq_img);
    cout << "[BASARILI] Yeni goruntu kaydedildi -> " << output_path << endl;

    return 0;
}
