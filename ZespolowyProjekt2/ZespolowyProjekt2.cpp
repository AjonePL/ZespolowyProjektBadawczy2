// ZespolowyProjekt2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Reader.h"
#include "Encoder.h"
#include "Decoder.h"
#include <cmath>
#include <opencv2/highgui.hpp>
#include <opencv2/core/core.hpp>
#define _CRT_SECURE_NO_WARNINGS

int main()
{

    Reader* rd = new Reader(std::string("1_3840.bmp"));
    std::vector<std::vector<int>> image = rd->GetImage();

    Encoder* en = new Encoder(image,true);
    en->EncodeToFile("test123.txt");

    Decoder* de = new Decoder(en->GetAvgOfSquaresVec(), en->GetIndexes(), en->GetDictionary(), true);
    de->Decode();
    de->Plot();


    /*cv::Mat showImage(image.size(), image.at(0).size(), CV_8U);
    for (int i = 0; i < showImage.rows; ++i)
        for (int j = 0; j < showImage.cols; ++j)
            showImage.at<uchar>(i, j) = image[i][j];
    cv::imwrite("test.bmp", showImage);*/


    float PSNR = 0;
    float MSE = 0;
    float maxOrg = 0;
    std::vector<std::vector<uchar>> wynik = de->GetImageInput();
    for (int i = 0; i < wynik.size(); i++) {
        for (int j = 0; j < wynik[0].size(); j++) {
            MSE += (image[i][j] - wynik[i][j]) * (image[i][j] - wynik[i][j]);
            if (image[i][j] > maxOrg)
                maxOrg = image[i][j];
        }
    }
    MSE = MSE / (wynik.size() * wynik[0].size());
    PSNR = 10 * log10(maxOrg * maxOrg / MSE);
    std::cout << PSNR << std::endl;
    return 0;
}