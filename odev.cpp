#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {
    // Senin bash kısmına yapıştırdığın fotoğraf yolu buraya otomatik eklenecek
    string img_path = "/content/resimler/c3045ae6-a3c5-4744-b342-2b317924f39a.jpeg";
    
    Mat img = imread(img_path);
    if (img.empty()) {
        cout << "HATA: Fotograf okunamadi! Yol yanlis olabilir: " << img_path << endl;
        return -1;
    }

    // 1024x768 Boyutlandirma Islemi
    Mat resized_img;
    resize(img, resized_img, Size(1024, 768));

    // Yeni fotografi kaydet
    string output_name = "boyutlandirilmis_resim.jpg";
    imwrite(output_name, resized_img);
    
    cout << "[BASARILI] Fotograf 1024x768 boyutuna getirildi ve '" << output_name << "' olarak kaydedildi!" << endl;
    
    return 0;
}
