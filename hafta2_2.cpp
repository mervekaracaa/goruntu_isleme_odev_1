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
void calculateLocalHistogram(const Mat& src, int startRow, int endRow, vector<int>& localHist) {
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
    int step = img.rows / 4;
    
    cout << "=== GOREV 2: 4 FARKLI PARLAKLIK DEGISTIRME (1 THREAD VS 4 THREAD) ===" << endl;
    
    // --- 1 THREAD İLE 4 FARKLI PARLAKLIK ---
    // (Aynı işlemleri sırayla tek işlemci çekirdeğinde yapıyoruz)
    auto start = high_resolution_clock::now();
    changeBrightness(img, dst1, 0, step,        1.0, 10);  // 1. Bölgeye +10 Parlaklık
    changeBrightness(img, dst1, step, step*2,   1.0, 30);  // 2. Bölgeye +30 Parlaklık
    changeBrightness(img, dst1, step*2, step*3, 1.0, 50);  // 3. Bölgeye +50 Parlaklık
    changeBrightness(img, dst1, step*3, img.rows, 1.0, 70); // 4. Bölgeye +70 Parlaklık
    auto end = high_resolution_clock::now();
    auto duration1 = duration_cast<milliseconds>(end - start).count();
    cout << "1 Thread ile 4 Farkli Parlaklik Degisimi Suresi: " << duration1 << " ms" << endl;

    // --- 4 THREAD İLE 4 FARKLI PARLAKLIK ---
    // (Aynı işlemleri 4 farklı işlemci çekirdeğinde aynı anda yapıyoruz)
    start = high_resolution_clock::now();
    thread t1(changeBrightness, cref(img), ref(dst4), 0, step,        1.0, 10);
    thread t2(changeBrightness, cref(img), ref(dst4), step, step*2,   1.0, 30);
    thread t3(changeBrightness, cref(img), ref(dst4), step*2, step*3, 1.0, 50);
    thread t4(changeBrightness, cref(img), ref(dst4), step*3, img.rows, 1.0, 70);
    t1.join(); t2.join(); t3.join(); t4.join();
    end = high_resolution_clock::now();
    auto duration4 = duration_cast<milliseconds>(end - start).count();
    cout << "4 Thread ile 4 Farkli Parlaklik Degisimi Suresi: " << duration4 << " ms" << endl;
    cout << "--------------------------------------------------------" << endl;

    cout << "=== GOREV 3 & 4: HISTOGRAM HESAPLAMA (1 THREAD VS 4 THREAD) ===" << endl;
    
    vector<int> hist1(256, 0);
    start = high_resolution_clock::now();
    calculateLocalHistogram(img, 0, img.rows, hist1);
    end = high_resolution_clock::now();
    auto hist_dur1 = duration_cast<milliseconds>(end - start).count();
    cout << "1 Thread ile Histogram Hesaplama Suresi: " << hist_dur1 << " ms" << endl;

    vector<int> h1(256,0), h2(256,0), h3(256,0), h4(256,0), hist4(256,0);
    start = high_resolution_clock::now();
    thread ht1(calculateLocalHistogram, cref(img), 0, step, ref(h1));
    thread ht2(calculateLocalHistogram, cref(img), step, step*2, ref(h2));
    thread ht3(calculateLocalHistogram, cref(img), step*2, step*3, ref(h3));
    thread ht4(calculateLocalHistogram, cref(img), step*3, img.rows, ref(h4));
    ht1.join(); ht2.join(); ht3.join(); ht4.join();
    
    for(int i=0; i<256; i++) {
        hist4[i] = h1[i] + h2[i] + h3[i] + h4[i];
    }
    end = high_resolution_clock::now();
    auto hist_dur4 = duration_cast<milliseconds>(end - start).count();
    cout << "4 Thread ile Histogram Hesaplama ve Birlestirme Suresi: " << hist_dur4 << " ms" << endl;
    cout << "--------------------------------------------------------" << endl;

    cout << "=== GOREV 5: OZEL PARLAKLIK ISLEMLERI ===" << endl;
    Mat finalImg1 = img.clone();
    changeBrightness(img, finalImg1, 0, img.rows, 0.75, 20);
    imwrite("/content/yeniboyut_resimler/gorev5_carpim075_arti20.jpg", finalImg1);
    cout << "[KAYDEDILDI] gorev5_carpim075_arti20.jpg" << endl;

    Mat finalImg2 = img.clone();
    changeBrightness(img, finalImg2, 0, img.rows, 0.75, 0);
    imwrite("/content/yeniboyut_resimler/gorev5_sadece_carpim075.jpg", finalImg2);
    cout << "[KAYDEDILDI] gorev5_sadece_carpim075.jpg" << endl;

    return 0;
}
