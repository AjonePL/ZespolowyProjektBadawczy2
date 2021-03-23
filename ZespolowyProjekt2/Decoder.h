#pragma once
#include<vector>
#include <opencv2/opencv.hpp>
#include <string>
class Decoder
{
public:
	Decoder(std::vector<int> AvgOfSquaresVec, std::vector<int> DicIndexOfSquaresVec, std::vector<std::vector<int>> Dictionary,bool uhd) {
		mAvgOfSquaresVec = AvgOfSquaresVec;
		mDicIndexOfSquaresVec = DicIndexOfSquaresVec;
		mDictionary = Dictionary;
		if (!uhd) {
			std::vector<std::vector<uchar>> ImageInput(1080, std::vector<uchar>(1920, 0));
			mImageInput = ImageInput;
		}
		else {
			std::vector<std::vector<uchar>> ImageInput(2160, std::vector<uchar>(3840, 0));
			mImageInput = ImageInput;
		}
	}
	~Decoder() {
		mAvgOfSquaresVec.clear();
		mDicIndexOfSquaresVec.clear();
		mDictionary.clear();
		mImageInput.clear();
	};
	bool Decode();
	bool Plot();
	bool SaveToFile(std::string filename);
	std::vector<std::vector<uchar>> GetImageInput() {
		return mImageInput;
	}



private:
	std::vector<int>	mAvgOfSquaresVec;
	std::vector<int>	mDicIndexOfSquaresVec;
	std::vector<std::vector<int>> mDictionary;
	cv::Mat mImage;
	std::vector<std::vector<uchar>> mImageInput;
};

