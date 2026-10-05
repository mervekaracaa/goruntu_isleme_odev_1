#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {
    string img_path = string(getenv("FOTOGRAF_YOLU"));
    // Görüntüyü tek kanallı (Grayscale) açıyoruz
    Mat img = imread(img_path, IMREAD_GRAYSCALE);
    if (img.empty()) {
        cout << "HATA: Fotograf okunamadi! Lutfen yolu kontrol et." << endl;
        return -1;
    }
    
    cout << "=== 3. HAFTA: KONTRAST GERME (LINEAR SCALING) ===" << endl;

    // 1. ADIM: Min ve Max değerleri bulma (Hazır fonksiyon KULLANMADAN)
    int min_val = 255;
    int max_val = 0;
    
    for (int r = 0; r < img.rows; r++) {
        for (int c = 0; c < img.cols; c++) {
            int val = img.at<uchar>(r, c);
            if (val < min_val) min_val = val;
            if (val > max_val) max_val = val;
        }
    }
    
    cout << "Görüntüdeki Minimum Piksel Değeri: " << min_val << endl;
    cout << "Görüntüdeki Maksimum Piksel Değeri: " << max_val << endl;

    if (min_val == max_val) {
        cout << "HATA: Min ve Max degerleri ayni, goruntu tek renk. Olcekleme yapilamaz!" << endl;
        return -1;
    }

    // 2. ADIM: [0, 255] aralığına doğrusal ölçekleme (Hazır fonksiyon KULLANMADAN)
    Mat stretched_img = img.clone();
    
    for (int r = 0; r < img.rows; r++) {
        for (int c = 0; c < img.cols; c++) {
            int pixel = img.at<uchar>(r, c);
            // Doğrusal Ölçekleme Formülü: Yeni = ((Eski - Min) * 255) / (Max - Min)
            int new_pixel = ((pixel - min_val) * 255) / (max_val - min_val);
            stretched_img.at<uchar>(r, c) = saturate_cast<uchar>(new_pixel);
        }
    }

    // 3. ADIM: Yeni görüntüyü Hafta_3 klasörüne kaydetme
    string output_path = "/content/Hafta_3/kontrast_artirilmis.jpg";
    imwrite(output_path, stretched_img);
    cout << "[BASARILI] Yeni goruntu kaydedildi -> " << output_path << endl;

    return 0;
}
