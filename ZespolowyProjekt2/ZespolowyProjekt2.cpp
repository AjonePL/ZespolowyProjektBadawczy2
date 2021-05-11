// ZespolowyProjekt2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Reader.h"
#include "Encoder.h"
#include "Decoder.h"
#include <cmath>
#include <opencv2/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/highgui/highgui_c.h>
#include <ctime>
#define CVPLOT_HEADER_ONLY
#include <CvPlot/cvplot.h>
#define _CRT_SECURE_NO_WARNINGS
struct indexWystapienia {
    int index;
    int wystapienia;
};

indexWystapienia DrawHistogram(int inDictionarySize, std::vector<int> indexes,std::string filename) {
    std::vector<float> histogram;
    histogram.resize(inDictionarySize);
    for (int i = 0; i < indexes.size(); i++) {
        histogram[indexes[i]] += 1;
    }
    int maxnum = 0;
    int indeks = 0;
    for (int i = 0; i < histogram.size(); i++) {
        if (maxnum < histogram[i]) {
            maxnum = histogram[i];
            indeks = i;
        }
    }
    /* Normalizacja
    float histSum = 0;
    for (int i = 0; i < histogram.size(); i++) {
        histSum += histogram[i];
    }*/
    for (int i = 0; i < histogram.size(); i++) {
        histogram[i] = histogram[i]; // podziel przez histSum przy normalizacji
    }



    std::sort(histogram.begin(), histogram.end(), std::greater<float>());

    auto axes2 = CvPlot::plot(histogram);
    cv::Mat mat = axes2.render(800, 1024);
    //CvPlot::show("mywindow", axes2);
    cv::imshow("mywindow", mat);

    filename = "Hist_" + filename;
    cv::imwrite(filename, mat);
    int k = cv::waitKey(0);
    indexWystapienia maxIndex;
    maxIndex.index = indeks;
    maxIndex.wystapienia = maxnum;
    return maxIndex;
}

int main()
{
    int dictionarySize = 4096;
    Encoder::EInitializationMode mode = Encoder::ePNN;
    bool checkIteration = false;

    std::vector<std::string> filenames;
    filenames.push_back("1_1920.bmp");
    //filenames.push_back("1_3840.bmp");
    //filenames.push_back("2_1920.bmp");
    //filenames.push_back("3_1920.bmp");
    //filenames.push_back("4_1920.bmp");
    /*filenames.push_back("5_1920.bmp");
    filenames.push_back("6_1920.bmp");
    filenames.push_back("7_1920.bmp");
    filenames.push_back("8_1920.bmp");
    filenames.push_back("9_1920.bmp");
    filenames.push_back("10_1920.bmp");
    filenames.push_back("11_1920.bmp");
    filenames.push_back("12_1920.bmp");
    filenames.push_back("13_1920.bmp");
    filenames.push_back("14_1920.bmp");
    filenames.push_back("15_1920.bmp");
    filenames.push_back("16_1920.bmp");
    filenames.push_back("17_1920.bmp");
    filenames.push_back("18_1920.bmp");
    filenames.push_back("19_1920.bmp");
    filenames.push_back("20_1920.bmp");

    filenames.push_back("2_3840.bmp");
    filenames.push_back("3_3840.bmp");
    filenames.push_back("4_3840.bmp");
    filenames.push_back("5_3840.bmp");
    filenames.push_back("6_3840.bmp");
    filenames.push_back("7_3840.bmp");
    filenames.push_back("8_3840.bmp");
    filenames.push_back("9_3840.bmp");
    filenames.push_back("10_3840.bmp");
    filenames.push_back("11_3840.bmp");
    filenames.push_back("12_3840.bmp");
    filenames.push_back("13_3840.bmp");
    filenames.push_back("14_3840.bmp");
    filenames.push_back("15_3840.bmp");
    filenames.push_back("16_3840.bmp");
    filenames.push_back("17_3840.bmp");
    filenames.push_back("18_3840.bmp");
    filenames.push_back("19_3840.bmp");
    filenames.push_back("20_3840.bmp");
    */
    
    for (int i = 0; i < filenames.size(); i++) {
        std::string filename = filenames[i];
        std::cout << i << ". " << filename << " ";
        Reader* rd = new Reader(filename);
        std::vector<std::vector<int>> image = rd->GetImage();
        bool isUHD = true;
        if (image.size() != 2160)
            isUHD = false;

        Encoder* en = new Encoder(image, mode, dictionarySize, checkIteration);
        clock_t start = clock();
        en->EncodeToFile("test123.txt");
        std::cout << "Czas: " << clock() - start << "ms ";
        std::vector<std::vector<int>> dictionary = en->GetDictionary();
        Decoder* de = new Decoder(en->GetAvgOfSquaresVec(), en->GetIndexes(), dictionary, isUHD);
        de->Decode();
        //de->Plot();

       indexWystapienia indWys = DrawHistogram(dictionarySize, en->GetIndexes(), filename);
       int iloscNiepustych = 0;
       for (int j = 0; j < dictionary.size(); j++) {
           if (dictionary[j].size() > 0)
               iloscNiepustych++;
       }
       std::cout << "Rozmiar slownika: " << iloscNiepustych << std::endl;
       std::cout << indWys.wystapienia << " ";
       
       for (int j = 0; j < 16; j++) {
           std::cout << dictionary[indWys.index][j] << " ";
       }
       std::cout << std::endl;
       de->PrintMSEandPSNR(image);
       de->SaveToFile("Decoded_" + filename);
       delete de;
       delete en;
       delete rd;
    }
    return 0;
}