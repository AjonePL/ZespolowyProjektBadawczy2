#include "Encoder.h"
#include <algorithm>
#include <iostream>
#include <random>
#include <limits> 
#include <thread>
#include <functional>




Encoder::Encoder(std::vector<std::vector<int>> image, bool isUHD)
{
	mUHD = isUHD;
	mImageVec2 = image;
}

bool Encoder::EncodeToFile(std::string filename)
{
	DivideImage();
	SelectFirstDictionary();
	LBGAlgorithm(2);
	return false;
}

bool Encoder::DivideImage() {
	for (int i = 0; i < mImageVec2.size(); i += 4) {
		for (int j = 0; j < mImageVec2[0].size(); j += 4) {
			mSquaresVec.push_back(std::vector<int>());
			mDicIndexOfSquaresVec.push_back(0);
			mAvgOfSquaresVec.push_back(0);
			for (int k = 0; k < 4; k++) {
				for (int l = 0; l < 4; l++) {
					mSquaresVec[mSquaresVec.size() - 1].push_back(mImageVec2[i + k][j + l]);
					mAvgOfSquaresVec[mSquaresVec.size() - 1] += mImageVec2[i + k][j + l];
				}
			}
			mAvgOfSquaresVec[mSquaresVec.size() - 1] = mAvgOfSquaresVec[mSquaresVec.size() - 1] / 16;
			for (int k = 0; k < 16; k++) {
				mSquaresVec[mSquaresVec.size() - 1][k] -= mAvgOfSquaresVec[mSquaresVec.size() - 1];
			}
		}
	}
	return true;
}


bool Encoder::SelectFirstDictionary() {
	int dictionarySize;
	if (!mUHD)
		dictionarySize = 1024;
	else
		dictionarySize = 4096;
	std::vector<std::vector<int>> out;
	std::sample(
		mSquaresVec.begin(),
		mSquaresVec.end(),
		std::back_inserter(mDictionary),
		dictionarySize,
		std::mt19937{ std::random_device{}() }
	);

	return true;
}

bool Encoder::LBGAlgorithm(int numIter)
{
	for (int k = 0; k < numIter+1; k++) {
		std::cout << k << std::endl;
		FindBestIndexes();
		if(k< numIter)
			ImproveDictionary();
	}
	return true;
}

bool Encoder::FindBestIndexes()
{
	std::vector<std::thread> multi;
	for (int i = 0; i < 4; i++) {
		multi.push_back(std::thread(&Encoder::MultiSelect,this,(i * mSquaresVec.size() / 4), ((i + 1) * mSquaresVec.size() / 4)));
	}
	for (int i = 0; i < 4; i++) {
		multi[i].join();
		}
	return true;
}

bool Encoder::ImproveDictionary()
{
	std::vector<std::vector<int>> newDictionary;
	for (int i = 0; i < mDictionary.size(); i++) {
		int k = 0;
		for (int j = 0; j < mDicIndexOfSquaresVec.size(); j++) {
			if (mDicIndexOfSquaresVec[j] == i) {
				if (k==0)
					newDictionary.push_back(mSquaresVec[j]);
				else {
					std::transform(newDictionary[i].begin(), newDictionary[i].end(), mSquaresVec[j].begin(), newDictionary[i].begin(), std::plus<int>());
				}
				k++;
			}
		}
		if (k == 0) {
			newDictionary.push_back(mDictionary[i]);
		}
		else {
			for (int j = 0; j < newDictionary[i].size(); j++)
				newDictionary[i][j] = newDictionary[i][j] / k;
		}
	}
	mDictionary = newDictionary;
	newDictionary.clear();
	return true;
}

void Encoder::MultiSelect(int x, int y)
{
	for (int i = x; i < y; i++) {
		int minSum = std::numeric_limits<int>::max();
		for (int j = 0; j < mDictionary.size(); j++) {
			int sum = 0;
			for (int k = 0; k < mDictionary[0].size(); k++) {
				sum += (mDictionary[j][k] - mSquaresVec[i][k]) * (mDictionary[j][k] - mSquaresVec[i][k]);
			}
			if (sum < minSum) {
				mDicIndexOfSquaresVec[i] = j;
				minSum = sum;
			}
		}
	}
}
