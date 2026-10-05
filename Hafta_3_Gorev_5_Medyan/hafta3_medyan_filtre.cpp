#include <iostream>
#include <vector>
#include <algorithm> // Sıralama (sort) işlemi için
#include <opencv2/opencv.hpp>
#include <cstdlib>

using namespace std;
using namespace cv;

// --- 1. ADIM: SENSÖR BOZULMASI SİMÜLASYONU (TUZ VE KARABİBER GÜRÜLTÜSÜ) ---
Mat tuzVeKarabiberGurultusuEkle(Mat img, float gurultuOrani) {
    Mat gurultulu = img.clone();
    int pikselSayisi = img.rows * img.cols;
    int bozukSensorSayisi = pikselSayisi * gurultuOrani; 
    
    // Sensör ölümleri: Ya tam siyah (0) ya da tam beyaz (255) değer üretir
    for(int i = 0; i < bozukSensorSayisi; i++) {
        int rX = rand() % img.cols;
        int rY = rand() % img.rows;
        
        if(rand() % 2 == 0) {
            gurultulu.at<uchar>(rY, rX) = 0;   // Ölü sensör (Karabiber)
        } else {
            gurultulu.at<uchar>(rY, rX) = 255; // Kısa devre sensör (Tuz)
        }
    }
    return gurultulu;
}

// --- 2. ADIM: KENDİ YAZDIĞIMIZ 5x5 MEDYAN FİLTRESİ ---
Mat kendiMedyanFiltrem(Mat img) {
    Mat sonucImg = img.clone();
    int yukseklik = img.rows;
    int genislik = img.cols;
    
    // 5x5 pencerenin merkezden uzaklığı 2 pikseldir (2 sol, 2 sağ vb.)
    int yariCap = 2; 

    // Kenarlardan taşıp hata vermemesi için döngüyü içeriden başlatıyoruz
    for(int y = yariCap; y < yukseklik - yariCap; y++) {
        for(int x = yariCap; x < genislik - yariCap; x++) {
            
            vector<uchar> komsular;
            
            // 5x5'lik alandaki tüm değerleri (25 adet pikseli) vektöre atıyoruz
            for(int i = -yariCap; i <= yariCap; i++) {
                for(int j = -yariCap; j <= yariCap; j++) {
                    komsular.push_back(img.at<uchar>(y + i, x + j));
                }
            }
            
            // NOT: Medyan filtreler, değerleri sıraladığı için konvolüsyon 
            // filtrelerine göre daha yavaştır. 
            // Burada C++'ın kendi sort fonksiyonunu kullanıyoruz (OpenCV değil).
            sort(komsular.begin(), komsular.end());
            
            // 25 elemanlı bir dizinin ortasındaki (medyan) değer 12. indekstir
            sonucImg.at<uchar>(y, x) = komsular[12];
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
    
    cout << "=== 3. HAFTA GOREV 5: MEDYAN FILTRE (5x5) ===" << endl;

    // Önce resmi bozuyoruz (%10 oranında sensör arızası)
    Mat gurultuluResim = tuzVeKarabiberGurultusuEkle(img, 0.10);
    imwrite("/content/Hafta_3_Gorev_5_Medyan/1_gurultulu_resim.jpg", gurultuluResim);
    cout << "[1/3] Resme Tuz ve Karabiber gurultusu (Olu Sensor) eklendi." << endl;

    // 1. OpenCV Hazır Fonksiyonu (5x5)
    Mat opencv_medyan;
    medianBlur(gurultuluResim, opencv_medyan, 5);
    imwrite("/content/Hafta_3_Gorev_5_Medyan/2_opencv_medyan.jpg", opencv_medyan);
    cout << "[2/3] OpenCV 'medianBlur' calisti." << endl;

    // 2. Kendi Yazdığımız Fonksiyon (5x5)
    Mat manuel_medyan = kendiMedyanFiltrem(gurultuluResim);
    imwrite("/content/Hafta_3_Gorev_5_Medyan/3_manuel_medyan.jpg", manuel_medyan);
    cout << "[3/3] Kendi yazdigimiz 5x5 Medyan Filtre calisti." << endl;

    return 0;
}
