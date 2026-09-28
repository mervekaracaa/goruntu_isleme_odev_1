#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {
    string folder_path = "resimler/*.jpg"; 
    vector<String> filenames;
    glob(folder_path, filenames, false);

    if (filenames.empty()) {
        cout << "Resim bulunamadi!" << endl;
        return -1;
    }

    for (size_t i = 0; i < filenames.size(); i++) {
        Mat img = imread(filenames[i]);
        if (img.empty()) continue;
        
        Mat resized_img;
        resize(img, resized_img, Size(1024, 768));
        imshow("Odev - 1024x768", resized_img);
        
        int key = waitKey(0);
        if (key == 27) break; // ESC tuşu çıkışı
    }
    return 0;
}
