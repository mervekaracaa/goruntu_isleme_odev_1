#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

// --- HOCANIN ISTEDIGI KENDI CLAHE FONKSIYONUMUZ ---
Mat kendiClaheFonksiyonum(Mat img) {
    int genislik = img.cols;
    int yukseklik = img.rows;

    // Resmi 8x8'lik parçalara bölüyoruz
    int hucreGenisligi = genislik / 8;
    int hucreYuksekligi = yukseklik / 8;

    // 8x8'lik ızgara için 256 değerlik histogram dizisi
    int hist[8][8][256] = {0};

    // 1. ADIM: Her bölgenin histogramını tek tek çıkaralım
    for(int i = 0; i < yukseklik; i++) {
        for(int j = 0; j < genislik; j++) {
            int bolgeX = j / hucreGenisligi;
            int bolgeY = i / hucreYuksekligi;

            // Sınır aşımını engellemek için (matris dışına çıkmamak için)
            if(bolgeX >= 8) bolgeX = 7;
            if(bolgeY >= 8) bolgeY = 7;

            int pikselDegeri = img.at<uchar>(i, j);
            hist[bolgeY][bolgeX][pikselDegeri]++;
        }
    }

    // 2. ADIM: Kırpma (Clipping) Limiti Uygulama
    // Formül = 2.0 * (Toplam Piksel) / 256
    int clipLimit = 2.0 * (hucreGenisligi * hucreYuksekligi) / 256;
    int cdf[8][8][256] = {0};

    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {

            // Sınırı aşan pikselleri bulalım
            int fazlalik = 0;
            for(int k = 0; k < 256; k++) {
                if(hist[i][j][k] > clipLimit) {
                    fazlalik += (hist[i][j][k] - clipLimit);
                    hist[i][j][k] = clipLimit;
                }
            }

            // Taşan pikselleri diğer değerlere eşit şekilde dağıtalım
            int dagitilacak = fazlalik / 256;
            for(int k = 0; k < 256; k++) {
                hist[i][j][k] += dagitilacak;
            }

            // Bu hücre için CDF (Kümülatif Dağılım) hesaplayalım
            cdf[i][j][0] = hist[i][j][0];
            for(int k = 1; k < 256; k++) {
                cdf[i][j][k] = cdf[i][j][k-1] + hist[i][j][k];
            }
        }
    }

    // 3. ADIM: Bilinear Interpolation (Piksellerin arası kare kare durmasın diye)
    Mat sonucImg = img.clone();

    for(int y = 0; y < yukseklik; y++) {
        for(int x = 0; x < genislik; x++) {
            int val = img.at<uchar>(y, x);

            // Hangi hücrenin merkezinde olduğumuzu buluyoruz
            float merkezX = (float)x / hucreGenisligi - 0.5;
            float merkezY = (float)y / hucreYuksekligi - 0.5;

            int x1 = merkezX;
            int y1 = merkezY;
            int x2 = x1 + 1;
            int y2 = y1 + 1;

            // Sınır kontrolleri
            if(x1 < 0) x1 = 0;
            if(y1 < 0) y1 = 0;
            if(x2 > 7) x2 = 7;
            if(y2 > 7) y2 = 7;

            float x_agirlik = merkezX - x1;
            float y_agirlik = merkezY - y1;

            // Kenarlardaysak ağırlığı sıfırlıyoruz
            if(merkezX < 0) { x_agirlik = 0; x2 = x1; }
            if(merkezY < 0) { y_agirlik = 0; y2 = y1; }

            // Formüle göre 4 köşe komşunun CDF değerlerini alıyoruz
            float c11 = (float)cdf[y1][x1][val] * 255 / (hucreGenisligi * hucreYuksekligi);
            float c12 = (float)cdf[y1][x2][val] * 255 / (hucreGenisligi * hucreYuksekligi);
            float c21 = (float)cdf[y2][x1][val] * 255 / (hucreGenisligi * hucreYuksekligi);
            float c22 = (float)cdf[y2][x2][val] * 255 / (hucreGenisligi * hucreYuksekligi);

            // Çift Doğrusal (Bilinear) oranlama formülü
            float ustKisim = c11 * (1 - x_agirlik) + c12 * x_agirlik;
            float altKisim = c21 * (1 - x_agirlik) + c22 * x_agirlik;
            float yeniPiksel = ustKisim * (1 - y_agirlik) + altKisim * y_agirlik;

            sonucImg.at<uchar>(y, x) = saturate_cast<uchar>(yeniPiksel);
        }
    }

    return sonucImg;
}

int main() {
    string img_path = string(getenv("FOTOGRAF_YOLU"));
    Mat img = imread(img_path, IMREAD_GRAYSCALE);
    if (img.empty()) {
        cout << "HATA: Fotograf okunamadi! Lutfen yolu kontrol et." << endl;
        return -1;
    }

    cout << "=== 3. HAFTA GOREV 3: CLAHE ===" << endl;

    // 1. OPENCV HAZIR FONKSIYONU KULLANARAK
    Ptr<CLAHE> clahe = createCLAHE(2.0, Size(8, 8));
    Mat opencv_clahe;
    clahe->apply(img, opencv_clahe);
    imwrite("/content/Hafta_3_Gorev_3_CLAHE/1_opencv_clahe_sonucu.jpg", opencv_clahe);
    cout << "Adim 1: Hazir OpenCV fonksiyonu calisti ve kaydedildi." << endl;

    // 2. KENDI YAZDIGIMIZ (MANUEL) FONKSIYONU KULLANARAK
    Mat manuel_clahe = kendiClaheFonksiyonum(img);
    imwrite("/content/Hafta_3_Gorev_3_CLAHE/2_manuel_clahe_sonucu.jpg", manuel_clahe);
    cout << "Adim 2: Kendi yazdigimiz CLAHE fonksiyonu calisti ve kaydedildi." << endl;

    return 0;
}
