#include "Decoder.h"

bool Decoder::Decode()
{
	int p = 0;
	for (int i = 0; i < mImageInput.size(); i += 4) {
		for (int j = 0; j < mImageInput[0].size(); j += 4) {
			for (int x = 0; x < 4; x++) {
				for (int y = 0; y < 4; y++) {
					
					if (mDictionary[mDicIndexOfSquaresVec[p]][x * 4 + y] + mAvgOfSquaresVec[p] < 0)
						mImageInput[i + x][j + y] = 0;
					else if (mDictionary[mDicIndexOfSquaresVec[p]][x * 4 + y] + mAvgOfSquaresVec[p] > 255)
						mImageInput[i + x][j + y] = 255;
					else
						mImageInput[i + x][j + y] = mDictionary[mDicIndexOfSquaresVec[p]][x * 4 + y] + mAvgOfSquaresVec[p];
				}
			}
			p++;
		}
	}
	cv::Mat showImage(mImageInput.size(), mImageInput.at(0).size(), CV_8U);
	for (int i = 0; i < showImage.rows; ++i)
		for (int j = 0; j < showImage.cols; ++j)
			showImage.at<uchar>(i, j) = mImageInput[i][j];

	mImage = showImage;
	return true;
}

bool Decoder::Plot()
{
	cv::imshow("test", mImage);
	int k = cv::waitKey(0);
	return false;
}

bool Decoder::SaveToFile(std::string filename)
{
	cv::imwrite(filename, mImage);
	return false;
}
