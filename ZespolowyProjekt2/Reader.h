#pragma once
#include <string>
#include<vector>
#include <opencv2/opencv.hpp>
class Reader
{
public:
	Reader(std::string filename);
	std::vector<std::vector<int>> GetImage();


private:
	bool ReadImage();
	bool ConvertToVector();
	int mWidth;
	int mHeight;
	cv::Mat mImage;
	bool mInitialized;

	std::vector<std::vector<int>> mImageOutput;
	std::string mFilename;
};

