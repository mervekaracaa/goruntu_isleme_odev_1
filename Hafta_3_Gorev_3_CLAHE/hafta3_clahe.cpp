#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat kendiClaheFonksiyonum(Mat img) {
    int genislik = img.cols;
    int yukseklik = img.rows;
    
    int hucreGenisligi = genislik / 8;
    int hucreYuksekligi = yukseklik / 8;
    
    int hist[8][8][256] = {0};
    
    for(int i = 0; i < yukseklik; i++) {
        for(int j = 0; j < genislik; j++) {
            int bolgeX = j / hucreGenisligi;
            int bolgeY = i / hucreYuksekligi;
            
            if(bolgeX >= 8) bolgeX = 7;
            if(bolgeY >= 8) bolgeY = 7;
            
            int pikselDegeri = img.at<uchar>(i, j);
            hist[bolgeY][bolgeX][pikselDegeri]++;
        }
    }
    
    int clipLimit = 2.0 * (hucreGenisligi * hucreYuksekligi) / 256;
    int cdf[8][8][256] = {0};
    
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            
            int fazlalik = 0;
            for(int k = 0; k < 256; k++) {
                if(hist[i][j][k] > clipLimit) {
                    fazlalik += (hist[i][j][k] - clipLimit);
                    hist[i][j][k] = clipLimit;
                }
            }
            
            int dagitilacak = fazlalik / 256;
            for(int k = 0; k < 256; k++) {
                hist[i][j][k] += dagitilacak;
            }
            
            cdf[i][j][0] = hist[i][j][0];
            for(int k = 1; k < 256; k++) {
                cdf[i][j][k] = cdf[i][j][k-1] + hist[i][j][k];
            }
        }
    }
    
    Mat sonucImg = img.clone();
    
    for(int y = 0; y < yukseklik; y++) {
        for(int x = 0; x < genislik; x++) {
            int val = img.at<uchar>(y, x);
            
            float merkezX = (float)x / hucreGenisligi - 0.5;
            float merkezY = (float)y / hucreYuksekligi - 0.5;
            
            int x1 = merkezX;
            int y1 = merkezY;
            int x2 = x1 + 1;
            int y2 = y1 + 1;
            
            if(x1 < 0) x1 = 0;
            if(y1 < 0) y1 = 0;
            if(x2 > 7) x2 = 7;
            if(y2 > 7) y2 = 7;
            
            float x_agirlik = merkezX - x1;
            float y_agirlik = merkezY - y1;
            
            if(merkezX < 0) { x_agirlik = 0; x2 = x1; }
            if(merkezY < 0) { y_agirlik = 0; y2 = y1; }
            
            float c11 = (float)cdf[y1][x1][val] * 255 / (hucreGenisligi * hucreYuksekligi);
            float c12 = (float)cdf[y1][x2][val] * 255 / (hucreGenisligi * hucreYuksekligi);
            float c21 = (float)cdf[y2][x1][val] * 255 / (hucreGenisligi * hucreYuksekligi);
            float c22 = (float)cdf[y2][x2][val] * 255 / (hucreGenisligi * hucreYuksekligi);
            
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
        cout << "HATA: Fotograf okunamadi! Yol: " << img_path << endl;
        return -1;
    }
    
    cout << "=== 3. HAFTA GOREV 3: CLAHE ===" << endl;

    Ptr<CLAHE> clahe = createCLAHE(2.0, Size(8, 8));
    Mat opencv_clahe;
    clahe->apply(img, opencv_clahe);
    imwrite("/content/Hafta_3_Gorev_3_CLAHE/1_opencv_clahe_sonucu.jpg", opencv_clahe);
    cout << "Adim 1: Hazir OpenCV fonksiyonu calisti ve kaydedildi." << endl;

    Mat manuel_clahe = kendiClaheFonksiyonum(img);
    imwrite("/content/Hafta_3_Gorev_3_CLAHE/2_manuel_clahe_sonucu.jpg", manuel_clahe);
    cout << "Adim 2: Kendi yazdigimiz CLAHE fonksiyonu calisti ve kaydedildi." << endl;

    return 0;
}
