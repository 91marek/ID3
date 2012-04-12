/*
 * DecisionTree.cpp
 */

#define DEBUG

#ifdef DEBUG
#include <iostream>
#endif

#include "DecisionTree.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
using namespace id3lib;
using namespace std;
using namespace boost;

DecisionTree::DecisionTree() {

}

DecisionTree::~DecisionTree() {

}

void DecisionTree::build(const Table& examples, size_t categoryIndex,
		const std::string& missingValueMark) {
	// Utworzenie tablicy na przyklady
	shared_array<shared_array<size_t> > ex(
			new shared_array<size_t>[examplesCount_]);
	for (size_t i = 0; i < examplesCount_; ++i) {
		shared_array<size_t> tmp(new size_t[attributesCount_]);
		ex[i] = tmp;
	}

	// Mapowanie stringow na size_t
	// oraz zapamietywanie wystepujacych wartosci atrybutow
	for (size_t i = 0; i < attributesCount_; ++i) {
		for (size_t j = 0; j < examplesCount_; ++j) {
			size_t k = 0;
			for (; k < values_[i].size(); ++k) { // sprawdzenie czy taka wartosc juz wystapila
				if (0 == examples.getValueAt(j, i).compare(values_[i][k]))
					break;
			}
			if (k < values_[i].size()) // taka wartosc juz wystapila
				ex[j][i] = k;
			else { // zapamietanie nowej wartosci
				values_[i].push_back(examples.getValueAt(j, i));
				ex[j][i] = values_[i].size() - 1;
			}
		}
	}

#ifdef DEBUG
	// Wypisanie wystepujacych wartosci atrybutow
	cout << "values_:" << endl;
	for (size_t i = 0; i < values_.size(); ++i)
		for (size_t j = 0; j < values_[i].size(); ++j)
			cout << "i= " << i << " j=" << j << " " << values_[i][j] << endl;

	// Wypisanie tablicy na przyklady
	cout << "ex: " << endl;
	for (size_t i = 0; i < examplesCount_; ++i)
		for (size_t j = 0; j < attributesCount_; ++j)
			cout << "i=" << i << " j=" << j << " " << ex[i][j] << endl;
#endif
	/* niepotrzebne
	 // Destrukcja tablicy na przyklady
	 for (size_t i = 0; i < examplesCount_; ++i)
	 delete[] ex[i];
	 delete[] ex;
	 */
}

void DecisionTree::prune() {
	// TODO
	return;
}

void DecisionTree::prune(const Table& examples) {
	// TODO
	return;
}

vector<string> DecisionTree::classify(const Table& examples) const {
	// TODO
	return vector<string>();
}
