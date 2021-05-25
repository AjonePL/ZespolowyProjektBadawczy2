#include "Encoder.h"
#include <algorithm>
#include <iostream>
#include <random>
#include <limits> 
#include <thread>
#include <functional>
#include "Decoder.h"


Encoder::Encoder(std::vector<std::vector<int>> image, EInitializationMode initializationMode, int inDictionarySize, bool inShowPSNR = false)
{
	mDictionarySize = inDictionarySize;
	mInitializationMode = initializationMode;
	mImageVec2 = image;
	mShowPSNR = inShowPSNR;
}

bool Encoder::EncodeToFile(std::string filename)
{
	DivideImage();
	SelectFirstDictionary();
	LBGAlgorithm(mShowPSNR ? 100 : 20);
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

bool Encoder::SelectFirstDictionary()
{
	switch (mInitializationMode) {
	case eRandom:
		SelectFirstRandom();
		break;
	case ePNN:
		SelectFirstPNN();
		break;
	default:
			return false;
		break;
	}
	return true;
}


bool Encoder::SelectFirstRandom() {
	clock_t start = clock();
	std::vector<std::vector<int>> out;
	std::sample(
		mSquaresVec.begin(),
		mSquaresVec.end(),
		std::back_inserter(mDictionary),
		mDictionarySize,
		std::mt19937{ std::random_device{}() }
	);

	std::cout << "Czas Inicjalizacji: " << clock() - start << "ms " << std::endl;
	return true;
}

bool Encoder::SelectFirstPNN()
{
	clock_t start = clock();
	mDictionary.resize(mDictionarySize);
	std::vector<int> indexes(mSquaresVec.size(), 0);
	for (int i = 0; i < mSquaresVec.size(); i++) {
		std::vector<float> pAvgOfMacro2(8, 0.0);
		float pAvgOfSq = 0.0;
		std::vector<float> pAvgOfMacro4(4, 0.0);
		std::vector<float> pVarOfMacro4(4, 0.0);
		float pAvgVarOfSq = 0.0;
		for (int j = 0; j < 8; j++) {
			pAvgOfMacro2[j] = (mSquaresVec[i][2 * j] + mSquaresVec[i][2 * j + 1]) / 2;
			pAvgOfSq += pAvgOfMacro2[j];
		}
		pAvgOfSq = pAvgOfSq / 8;
		for (int j = 0; j < 8; j++) {
			indexes[i] = indexes[i] * 2;
			if (pAvgOfMacro2[j] >= pAvgOfSq)
				indexes[i]++;
		}
		pAvgOfMacro4[0] = (mSquaresVec[i][0] + mSquaresVec[i][1] + mSquaresVec[i][4] + mSquaresVec[i][5]) / 4;
		pAvgOfMacro4[1] = (mSquaresVec[i][2] + mSquaresVec[i][3] + mSquaresVec[i][6] + mSquaresVec[i][7]) / 4;
		pAvgOfMacro4[2] = (mSquaresVec[i][8] + mSquaresVec[i][9] + mSquaresVec[i][12] + mSquaresVec[i][13]) / 4;
		pAvgOfMacro4[3] = (mSquaresVec[i][10] + mSquaresVec[i][11] + mSquaresVec[i][14] + mSquaresVec[i][15]) / 4;
		pVarOfMacro4[0] = ((mSquaresVec[i][0] - pAvgOfMacro4[0]) * (mSquaresVec[i][0] - pAvgOfMacro4[0]) + (mSquaresVec[i][1] - pAvgOfMacro4[0]) * (mSquaresVec[i][1] - pAvgOfMacro4[0]) + (mSquaresVec[i][4] - pAvgOfMacro4[0]) * (mSquaresVec[i][4] - pAvgOfMacro4[0]) + (mSquaresVec[i][5] - pAvgOfMacro4[0]) * (mSquaresVec[i][5] - pAvgOfMacro4[0])) / 4;
		pVarOfMacro4[1] = ((mSquaresVec[i][2] - pAvgOfMacro4[1]) * (mSquaresVec[i][2] - pAvgOfMacro4[1]) + (mSquaresVec[i][3] - pAvgOfMacro4[1]) * (mSquaresVec[i][3] - pAvgOfMacro4[1]) + (mSquaresVec[i][6] - pAvgOfMacro4[1]) * (mSquaresVec[i][6] - pAvgOfMacro4[1]) + (mSquaresVec[i][7] - pAvgOfMacro4[1]) * (mSquaresVec[i][7] - pAvgOfMacro4[1])) / 4;
		pVarOfMacro4[2] = ((mSquaresVec[i][8] - pAvgOfMacro4[2]) * (mSquaresVec[i][8] - pAvgOfMacro4[2]) + (mSquaresVec[i][9] - pAvgOfMacro4[2]) * (mSquaresVec[i][9] - pAvgOfMacro4[2]) + (mSquaresVec[i][12] - pAvgOfMacro4[2]) * (mSquaresVec[i][12] - pAvgOfMacro4[2]) + (mSquaresVec[i][13] - pAvgOfMacro4[2]) * (mSquaresVec[i][13] - pAvgOfMacro4[2])) / 4;
		pVarOfMacro4[3] = ((mSquaresVec[i][10] - pAvgOfMacro4[3]) * (mSquaresVec[i][10] - pAvgOfMacro4[3]) + (mSquaresVec[i][11] - pAvgOfMacro4[3]) * (mSquaresVec[i][11] - pAvgOfMacro4[3]) + (mSquaresVec[i][14] - pAvgOfMacro4[3]) * (mSquaresVec[i][14] - pAvgOfMacro4[3]) + (mSquaresVec[i][15] - pAvgOfMacro4[3]) * (mSquaresVec[i][15] - pAvgOfMacro4[3])) / 4;
		pAvgVarOfSq = (pVarOfMacro4[0] + pVarOfMacro4[1] + pVarOfMacro4[2] + pVarOfMacro4[3]) / 4;
		for (int j = 0; j < 4; j++) {
			indexes[i] = indexes[i] * 2;
			if (pVarOfMacro4[j] >= pAvgVarOfSq)
				indexes[i]++;
		}
	}
	mDicIndexOfSquaresVec = indexes;
	ImproveDictionary();
	for (int i = 0; i < mDictionary.size(); i++) {
		while (mDictionary[i].size() == 0) {
			int index = rand() % mSquaresVec.size();
			auto it = std::find(std::begin(mDictionary), std::end(mDictionary), mSquaresVec[index]);
			if (it == std::end(mDictionary)) {
				mDictionary[i] = mSquaresVec[index];
			}
		}

	}
	
	std::cout << "Czas Inicjalizacji: " << clock() - start << "ms "<<std::endl;
	return true;
}

bool Encoder::LBGAlgorithm(int numIter)
{
	for (int k = 0; k < numIter + 1; k++) {
		FindBestIndexes();
		if ((k+1)%5==0 && mShowPSNR) {
			std::cout << "Iteracja " << (k + 1) << " :" << std::endl;
			Decoder* de = new Decoder(mAvgOfSquaresVec, mDicIndexOfSquaresVec, mDictionary, mUHD);
			de->Decode();
			de->PrintMSEandPSNR(mImageVec2);
		}
		if (k < numIter)
			ImproveDictionary();
	}
	return true;
}

bool Encoder::FindBestIndexes()
{
	std::vector<std::thread> multi;
	for (int i = 0; i < 4; i++) {
		multi.push_back(std::thread(&Encoder::MultiSelect, this, (i * mSquaresVec.size() / 4), ((i + 1) * mSquaresVec.size() / 4)));
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
				if (k == 0)
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
				newDictionary[i][j] = (int)round((float)newDictionary[i][j] / (float)k);
		}
	}
	/* puste
	for (int i = 0; i < newDictionary.size(); i++) {
		if(newDictionary[i].size() == 0){
			//newDictionary[i].resize(16, 0);
		}
	}
*/
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
			for (int k = 0; k < mDictionary[j].size(); k++) {
				sum += (mDictionary[j][k] - mSquaresVec[i][k]) * (mDictionary[j][k] - mSquaresVec[i][k]);
			}
			if (sum < minSum && mDictionary[j].size()>0) {
				mDicIndexOfSquaresVec[i] = j;
				minSum = sum;
			}
		}
	}
}
