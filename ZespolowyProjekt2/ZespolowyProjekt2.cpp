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
void DrawHistogram(bool isUHD, std::vector<int> indexes,std::string filename) {
    std::vector<float> histogram;
    if (isUHD)
        histogram.resize(4096);
    else
        histogram.resize(1024);
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
    filenames.push_back("2_1920.bmp");
    
    for (int i = 0; i < filenames.size(); i++) {
        std::string filename = filenames[i];
        std::cout << i << ". " << filename << " ";
        Reader* rd = new Reader(filename);
        std::vector<std::vector<int>> image = rd->GetImage();
        bool isUHD = true;
        if (image.size() != 2160)
            isUHD = false;

        Encoder* en = new Encoder(image, isUHD);
        clock_t start = clock();
        en->EncodeToFile("test123.txt");
        std::cout << "Czas: " << clock() - start << "ms ";
        Decoder* de = new Decoder(en->GetAvgOfSquaresVec(), en->GetIndexes(), en->GetDictionary(), isUHD);
        de->Decode();
        //de->Plot();

        DrawHistogram(isUHD, en->GetIndexes(), filename);
        PrintMSEandPSNR(image, de->GetImageInput());
        de->SaveToFile("Decoded_" + filename);
        delete de;
        delete en;
        delete rd;
    }
    return 0;
}