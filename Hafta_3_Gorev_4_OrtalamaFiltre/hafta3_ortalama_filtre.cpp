#include <iostream>
#include <opencv2/opencv.hpp>
#include <cstdlib> // Rastgele sayı üretmek için

using namespace std;
using namespace cv;

// --- 1. ADIM: RESME YAPAY GÜRÜLTÜ EKLEME FONKSİYONU ---
Mat gurultuEkle(Mat img) {
    Mat gurultulu = img.clone();
    int yukseklik = img.rows;
    int genislik = img.cols;
    int pikselSayisi = yukseklik * genislik;
    
    // Resmin %5'ine Tuz (Beyaz) ve Karabiber (Siyah) gürültüsü ekleyelim
    int gurultuMiktari = pikselSayisi * 0.05; 
    
    for(int i = 0; i < gurultuMiktari; i++) {
        int rX = rand() % genislik;
        int rY = rand() % yukseklik;
        int rastgeleDeger = rand() % 2; // 0 veya 1
        
        if(rastgeleDeger == 0) {
            gurultulu.at<uchar>(rY, rX) = 0;   // Karabiber (Siyah leke)
        } else {
            gurultulu.at<uchar>(rY, rX) = 255; // Tuz (Beyaz leke)
        }
    }
    return gurultulu;
}

// --- 2. ADIM: MANUEL 3x3 ORTALAMA FİLTRESİ (KONVOLÜSYON) ---
Mat manuelOrtalamaFiltre(Mat img) {
    Mat sonucImg = img.clone();
    int yukseklik = img.rows;
    int genislik = img.cols;
    
    // Kenarlarda 3x3 matris dışarı taşacağı için döngüyü 1'den başlatıp sondan 1 piksel önce bitiriyoruz
    for(int y = 1; y < yukseklik - 1; y++) {
        for(int x = 1; x < genislik - 1; x++) {
            
            int toplam = 0;
            
            // 3x3'lük çekirdeği (kernel) merkez pikselin etrafında gezdiriyoruz
            for(int i = -1; i <= 1; i++) {
                for(int j = -1; j <= 1; j++) {
                    toplam += img.at<uchar>(y + i, x + j);
                }
            }
            
            // Elde edilen toplamı 9'a bölerek ortalamayı buluyoruz (3x3 = 9 piksel)
            sonucImg.at<uchar>(y, x) = toplam / 9;
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
    
    cout << "=== 3. HAFTA GOREV 4: ORTALAMA FILTRE (MEAN FILTER) ===" << endl;

    // Önce resmi gürültülü hale getirip kaydediyoruz
    Mat gurultuluResim = gurultuEkle(img);
    imwrite("/content/Hafta_3_Gorev_4_OrtalamaFiltre/1_gurultulu_resim.jpg", gurultuluResim);
    cout << "Adim 1: Resme yapay gurultu eklendi ve kaydedildi." << endl;

    // Sonra yazdığımız manuel filtre ile gürültüyü temizliyoruz
    Mat filtrelenmisResim = manuelOrtalamaFiltre(gurultuluResim);
    imwrite("/content/Hafta_3_Gorev_4_OrtalamaFiltre/2_filtrelenmis_resim.jpg", filtrelenmisResim);
    cout << "Adim 2: 3x3 Ortalama Filtresi konvolusyon ile gezdirildi ve kaydedildi." << endl;

    return 0;
}
