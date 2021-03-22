#pragma once
#include <vector>
#include <string>

class Encoder
{
public:
	Encoder(std::vector<std::vector<int>> image, bool isUHD = false);
	bool EncodeToFile(std::string filename);
	std::vector<std::vector<int>> GetDictionary() {
		return mDictionary;
	}
	std::vector<int> GetAvgOfSquaresVec() {
		return mAvgOfSquaresVec;
	}
	std::vector<int> GetIndexes() {
		return mDicIndexOfSquaresVec;
	}
	std::vector<std::vector<int>> GetImageVec2() {
		return mImageVec2;
	}

private:
	bool DivideImage();
	bool SelectFirstDictionary();
	bool LBGAlgorithm(int numIter);
	bool FindBestIndexes();
	bool ImproveDictionary();
	void MultiSelect(int i, int x);
	
	bool mUHD;
	
	std::vector<int>	mAvgOfSquaresVec;
	std::vector<int>	mDicIndexOfSquaresVec;
	std::vector<std::vector<int>> mDictionary;
	std::vector<std::vector<int>> mImageVec2;
	std::vector<std::vector<int>> mSquaresVec;

};
