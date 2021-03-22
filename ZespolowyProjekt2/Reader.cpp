#include "Reader.h"
#include <iostream>
#include <cstddef>

#include <opencv2/opencv.hpp>


#define _CRT_SECURE_NO_WARNINGS

#define DATA_OFFSET_OFFSET 0x000A
#define WIDTH_OFFSET 0x0012
#define HEIGHT_OFFSET 0x0016
#define BITS_PER_PIXEL_OFFSET 0x001C

Reader::Reader(std::string filename) {
	mFilename = filename;
	
	try {
		ReadImage();
		ConvertToVector();
	}
	catch(...){
		mInitialized = false;
		return;
	}
	mInitialized = true;
}

std::vector<std::vector<int>> Reader::GetImage()
{
	if(mInitialized)
		return mImageOutput;
	return std::vector<std::vector<int>>();
}


bool Reader::ReadImage() {
	mImage = cv::imread(mFilename, cv::IMREAD_GRAYSCALE);
	cv::imshow("Display window", mImage);
	int k = cv::waitKey(0);
	mWidth = mImage.size().width;
	mHeight = mImage.size().height;
	return true;
}

bool Reader::ConvertToVector() {
	for (int i = 0; i < mHeight; i++) {
		mImageOutput.push_back(std::vector<int>());
		for (int j = 0; j < mWidth; j++) {
			mImageOutput[i].push_back(mImage.at<uchar>(i,j));
		}
	}

	return true;
}