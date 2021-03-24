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
#define _CRT_SECURE_NO_WARNINGS
void DrawHistogram(Encoder::EInitializationMode mode, std::vector<int> indexes,std::string filename) {
    std::vector<float> histogram;
    if (mode==Encoder::eRandom)
        histogram.resize(1024);
    else
        histogram.resize(4096);
    for (int i = 0; i < indexes.size(); i++) {
        histogram[indexes[i]] += 1;
    }
    float histMax = 0;
    for (int i = 0; i < histogram.size(); i++) {
        histogram[i] = histogram[i] / indexes.size();
        if (histMax < histogram[i])
            histMax = histogram[i];
    }
    for (int i = 0; i < histogram.size(); i++) {
        histogram[i] = ceil(700 * histogram[i] / histMax);
    }

    cv::Mat rook_image = cv::Mat::zeros(800, 1024, CV_8UC3);
    for (int i = 0; i < histogram.size() - 1; i++) {
        cv::line(rook_image,
            cv::Point(i, 800),
            cv::Point(i, 800 - histogram[i]),
            cv::Scalar(0, 255, 0),
            2,
            cv::LINE_8);
    }
    filename = "Hist_" + filename;
    cv::imwrite(filename, rook_image);
    int k = cv::waitKey(0);
}
void PrintMSEandPSNR(std::vector<std::vector<int>> image, std::vector<std::vector<uchar>> wynik) {
    float PSNR = 0;
    float MSE = 0;
    float maxOrg = 0;
    for (int i = 0; i < wynik.size(); i++) {
        for (int j = 0; j < wynik[0].size(); j++) {
            MSE += (image[i][j] - wynik[i][j]) * (image[i][j] - wynik[i][j]);
            if (image[i][j] > maxOrg)
                maxOrg = image[i][j];
        }
    }
    MSE = MSE / (wynik.size() * wynik[0].size());
    PSNR = 10 * log10(maxOrg * maxOrg / MSE);
    std::cout <<"MSE : "<<MSE<<" PSNR : "<< PSNR << std::endl;
}

int main()
{
    std::vector<std::string> filenames;
    filenames.push_back("1_1920.bmp");
    filenames.push_back("1_3840.bmp");
    /*filenames.push_back("2_1920.bmp");
    filenames.push_back("3_1920.bmp");
    filenames.push_back("4_1920.bmp");
    filenames.push_back("5_1920.bmp");
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
        Encoder::EInitializationMode mode = Encoder::ePNN;

        Encoder* en = new Encoder(image, mode);
        clock_t start = clock();
        en->EncodeToFile("test123.txt");
        std::cout << "Czas: " << clock() - start << "ms ";
        Decoder* de = new Decoder(en->GetAvgOfSquaresVec(), en->GetIndexes(), en->GetDictionary(), isUHD);
        de->Decode();
        //de->Plot();

        DrawHistogram(mode, en->GetIndexes(), filename);
        PrintMSEandPSNR(image, de->GetImageInput());
        de->SaveToFile("Decoded_" + filename);
        delete de;
        delete en;
        delete rd;
    }
    return 0;
}