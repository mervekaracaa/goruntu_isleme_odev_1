#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <functional>
#include <algorithm>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
using namespace std::chrono;

// --- Parcali Histogram Hesaplama Fonksiyonu ---
void calcHistLocal(const Mat& src, int startRow, int endRow, vector<int>& localHist) {
    fill(localHist.begin(), localHist.end(), 0);
    for (int r = startRow; r < endRow; r++) {
        for (int c = 0; c < src.cols; c++) {
            localHist[src.at<uchar>(r, c)]++;
        }
    }
}

int main() {
    string img_path = string(getenv("FOTOGRAF_YOLU"));
    Mat img = imread(img_path, IMREAD_GRAYSCALE);
    if (img.empty()) {
        cout << "HATA: Fotograf okunamadi! Lutfen yolu kontrol et." << endl;
        return -1;
    }
    
    int step = img.rows / 4;
    string output_folder = "/content/yeni_klasor_resimler";

    cout << "=== GOREV 2 & 3: HISTOGRAM HESAPLAMA (1 THREAD VS 4 THREAD) ===" << endl;
    
    // ---------------------------------------------------------
    // 1 THREAD İLE HESAPLAMA
    // ---------------------------------------------------------
    vector<int> hist1(256, 0);
    auto startH1 = high_resolution_clock::now();
    calcHistLocal(img, 0, img.rows, hist1);
    auto endH1 = high_resolution_clock::now();
    cout << "1 Thread ile Histogram Hesaplama Suresi: " 
         << duration_cast<milliseconds>(endH1 - startH1).count() << " ms" << endl;

    // ---------------------------------------------------------
    // 4 THREAD İLE HESAPLAMA VE BİRLEŞTİRME
    // ---------------------------------------------------------
    vector<int> h1(256,0), h2(256,0), h3(256,0), h4(256,0), hist4(256,0);
    auto startH4 = high_resolution_clock::now();
    
    thread th1(calcHistLocal, cref(img), 0, step, ref(h1));
    thread th2(calcHistLocal, cref(img), step, step*2, ref(h2));
    thread th3(calcHistLocal, cref(img), step*2, step*3, ref(h3));
    thread th4(calcHistLocal, cref(img), step*3, img.rows, ref(h4));
    
    th1.join(); th2.join(); th3.join(); th4.join();
    
    // Parcali histogramlari tek bir ana histogramda birlestir
    for(int i=0; i<256; i++) {
        hist4[i] = h1[i] + h2[i] + h3[i] + h4[i];
    }
    auto endH4 = high_resolution_clock::now();
    cout << "4 Thread ile Histogram (Hesaplama+Birlestirme) Suresi: " 
         << duration_cast<milliseconds>(endH4 - startH4).count() << " ms" << endl;

    // ---------------------------------------------------------
    // KENARDA GÖRÜNTÜ OLUŞTURMA: HISTOGRAM GRAFİĞİ ÇİZİMİ
    // ---------------------------------------------------------
    int hist_w = 512, hist_h = 400;
    int bin_w = cvRound((double) hist_w / 256);
    Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(255, 255, 255)); // Beyaz arka plan

    // Grafiği çizmeden önce değerleri resmin yüksekliğine göre normalize ediyoruz
    int max_val = *max_element(hist4.begin(), hist4.end());
    for(int i=0; i<256; i++) {
        hist4[i] = ((double)hist4[i] / max_val) * hist_h;
    }

    // Çizgi çizimi (Mavi renkli bir grafik çizgisi)
    for(int i = 1; i < 256; i++) {
        line(histImage, Point(bin_w*(i-1), hist_h - hist4[i-1]),
                        Point(bin_w*(i), hist_h - hist4[i]),
                        Scalar(255, 0, 0), 2, 8, 0);
    }

    // Çizilen grafiği yeni klasöre kaydet
    string output_path = output_folder + "/histogram_grafik_sonucu.jpg";
    imwrite(output_path, histImage);
    cout << "[RESIM OLUSTURULDU] -> " << output_path << endl;

    return 0;
}
