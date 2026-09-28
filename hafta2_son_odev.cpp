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

// --- GOREV 1: Parlaklik Degistirme Fonksiyonu ---
void applyBrightness(const Mat& src, Mat& dst, int startRow, int endRow, double alpha, int beta) {
    for (int r = startRow; r < endRow; r++) {
        for (int c = 0; c < src.cols; c++) {
            int val = src.at<uchar>(r, c) * alpha + beta;
            dst.at<uchar>(r, c) = saturate_cast<uchar>(val);
        }
    }
}

// --- GOREV 2 & 3: Parcali Histogram Hesaplama Fonksiyonu ---
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
    Mat dst1 = img.clone();
    Mat dst4 = img.clone();
    
    cout << "=== GOREV 1: 4 FARKLI PARLAKLIK ISLEMI (1 THREAD VS 4 THREAD) ===" << endl;
    
    // --- 1 THREAD İLE ---
    auto start1 = high_resolution_clock::now();
    applyBrightness(img, dst1, 0, step,        1.0, 10);
    applyBrightness(img, dst1, step, step*2,   1.0, 30);
    applyBrightness(img, dst1, step*2, step*3, 1.0, 50);
    applyBrightness(img, dst1, step*3, img.rows, 1.0, 70);
    auto end1 = high_resolution_clock::now();
    cout << "1 Thread ile 4 Farkli Parlaklik Suresi: " 
         << duration_cast<milliseconds>(end1 - start1).count() << " ms" << endl;

    // --- 4 THREAD İLE ---
    auto start4 = high_resolution_clock::now();
    thread t1(applyBrightness, cref(img), ref(dst4), 0, step,        1.0, 10);
    thread t2(applyBrightness, cref(img), ref(dst4), step, step*2,   1.0, 30);
    thread t3(applyBrightness, cref(img), ref(dst4), step*2, step*3, 1.0, 50);
    thread t4(applyBrightness, cref(img), ref(dst4), step*3, img.rows, 1.0, 70);
    t1.join(); t2.join(); t3.join(); t4.join();
    auto end4 = high_resolution_clock::now();
    cout << "4 Thread ile 4 Farkli Parlaklik Suresi: " 
         << duration_cast<milliseconds>(end4 - start4).count() << " ms" << endl;
         
    // Goruntuyu kenarda olusturulan yeni klasore kaydet
    imwrite(output_folder + "/4_farkli_parlaklik_sonucu.jpg", dst4);
    cout << "[RESIM OLUSTURULDU] -> yeni_klasor_resimler/4_farkli_parlaklik_sonucu.jpg" << endl;
    cout << "------------------------------------------------------------------" << endl;

    cout << "=== GOREV 2 & 3: HISTOGRAM HESAPLAMA (1 THREAD VS 4 THREAD) ===" << endl;
    
    // --- 1 THREAD HISTOGRAM ---
    vector<int> hist1(256, 0);
    auto startH1 = high_resolution_clock::now();
    calcHistLocal(img, 0, img.rows, hist1);
    auto endH1 = high_resolution_clock::now();
    cout << "1 Thread Histogram Hesaplama Suresi: " 
         << duration_cast<milliseconds>(endH1 - startH1).count() << " ms" << endl;

    // --- 4 THREAD HISTOGRAM VE BİRLEŞTİRME ---
    vector<int> h1(256,0), h2(256,0), h3(256,0), h4(256,0), hist4(256,0);
    auto startH4 = high_resolution_clock::now();
    thread th1(calcHistLocal, cref(img), 0, step, ref(h1));
    thread th2(calcHistLocal, cref(img), step, step*2, ref(h2));
    thread th3(calcHistLocal, cref(img), step*2, step*3, ref(h3));
    thread th4(calcHistLocal, cref(img), step*3, img.rows, ref(h4));
    th1.join(); th2.join(); th3.join(); th4.join();
    
    for(int i=0; i<256; i++) {
        hist4[i] = h1[i] + h2[i] + h3[i] + h4[i];
    }
    auto endH4 = high_resolution_clock::now();
    cout << "4 Thread Histogram (Hesaplama+Birlestirme) Suresi: " 
         << duration_cast<milliseconds>(endH4 - startH4).count() << " ms" << endl;

    return 0;
}
