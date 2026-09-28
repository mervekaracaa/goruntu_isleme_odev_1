#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
using namespace std::chrono;

// --- GOREV 2 & 5: Parlaklik Degistirme Fonksiyonu ---
void changeBrightness(const Mat& src, Mat& dst, int startRow, int endRow, float alpha, int beta) {
    for (int r = startRow; r < endRow; r++) {
        for (int c = 0; c < src.cols; c++) {
            int val = src.at<uchar>(r, c) * alpha + beta;
            dst.at<uchar>(r, c) = saturate_cast<uchar>(val);
        }
    }
}

// --- GOREV 3: Parcali Histogram Hesaplama Fonksiyonu ---
void calcHist(const Mat& src, int startRow, int endRow, vector<int>& localHist) {
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
        cout << "HATA: Fotograf okunamadi!" << endl;
        return -1;
    }
    
    Mat dst1 = img.clone();
    Mat dst4 = img.clone();
    
    cout << "=== GOREV 2: PARLAKLIK DEGISTIRME (1 THREAD VS 4 THREAD) ===" << endl;
    
    // 1 THREAD ILE PARLAKLIK (Rastgele bir parlaklik artisi: alpha=1.0, beta=30)
    auto start = high_resolution_clock::now();
    changeBrightness(img, dst1, 0, img.rows, 1.0, 30);
    auto end = high_resolution_clock::now();
    auto duration1 = duration_cast<milliseconds>(end - start).count();
    cout << "1 Thread ile Parlaklik Degisimi Suresi: " << duration1 << " ms" << endl;

    // 4 THREAD ILE PARLAKLIK
    int step = img.rows / 4;
    start = high_resolution_clock::now();
    thread t1(changeBrightness, cref(img), ref(dst4), 0, step, 1.0, 30);
    thread t2(changeBrightness, cref(img), ref(dst4), step, step*2, 1.0, 30);
    thread t3(changeBrightness, cref(img), ref(dst4), step*2, step*3, 1.0, 30);
    thread t4(changeBrightness, cref(img), ref(dst4), step*3, img.rows, 1.0, 30);
    t1.join(); t2.join(); t3.join(); t4.join();
    end = high_resolution_clock::now();
    auto duration4 = duration_cast<milliseconds>(end - start).count();
    cout << "4 Thread ile Parlaklik Degisimi Suresi: " << duration4 << " ms" << endl;
    cout << "--------------------------------------------------------" << endl;

    cout << "=== GOREV 3 & 4: HISTOGRAM HESAPLAMA (1 THREAD VS 4 THREAD) ===" << endl;
    
    // 1 THREAD ILE HISTOGRAM
    vector<int> hist1(256, 0);
    start = high_resolution_clock::now();
    calcHist(img, 0, img.rows, hist1);
    end = high_resolution_clock::now();
    auto hist_dur1 = duration_cast<milliseconds>(end - start).count();
    cout << "1 Thread ile Histogram Hesaplama Suresi: " << hist_dur1 << " ms" << endl;

    // 4 THREAD ILE HISTOGRAM
    vector<int> h1(256,0), h2(256,0), h3(256,0), h4(256,0), hist4(256,0);
    start = high_resolution_clock::now();
    thread ht1(calcHist, cref(img), 0, step, ref(h1));
    thread ht2(calcHist, cref(img), step, step*2, ref(h2));
    thread ht3(calcHist, cref(img), step*2, step*3, ref(h3));
    thread ht4(calcHist, cref(img), step*3, img.rows, ref(h4));
    ht1.join(); ht2.join(); ht3.join(); ht4.join();
    
    // Alt histogramlari ana histogramda birlestir
    for(int i=0; i<256; i++) {
        hist4[i] = h1[i] + h2[i] + h3[i] + h4[i];
    }
    end = high_resolution_clock::now();
    auto hist_dur4 = duration_cast<milliseconds>(end - start).count();
    cout << "4 Thread ile Histogram Hesaplama ve Birlestirme Suresi: " << hist_dur4 << " ms" << endl;
    cout << "--------------------------------------------------------" << endl;

    cout << "=== GOREV 5: OZEL PARLAKLIK ISLEMLERI ===" << endl;
    // a) 0.75 ile carp, 20 ekle
    Mat finalImg1 = img.clone();
    changeBrightness(img, finalImg1, 0, img.rows, 0.75, 20);
    imwrite("/content/yeniboyut_resimler/gorev5_carpim075_arti20.jpg", finalImg1);
    cout << "[KAYDEDILDI] gorev5_carpim075_arti20.jpg" << endl;

    // b) Sadece 0.75 ile carp
    Mat finalImg2 = img.clone();
    changeBrightness(img, finalImg2, 0, img.rows, 0.75, 0);
    imwrite("/content/yeniboyut_resimler/gorev5_sadece_carpim075.jpg", finalImg2);
    cout << "[KAYDEDILDI] gorev5_sadece_carpim075.jpg" << endl;

    return 0;
}
