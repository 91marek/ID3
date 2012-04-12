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

// TODO sensownie zainicjowac wszystkie pola
DecisionTree::DecisionTree() :
		values_(), attributes_(), examplesCount_(0), attributesCount_(0), categoryIndex_(
				0), missingValueMark_() {

}

DecisionTree::~DecisionTree() {

}

void DecisionTree::build(const Table& examples, size_t categoryIndex,
		const std::string& missingValueMark) {
	// Inicjalizacja
	vector<vector<string> > values_ = vector<vector<string> >(
			examples.columns());
	vector<string> attributes_ = examples.getAttr();
	examplesCount_ = examples.rows();
	attributesCount_ = examples.columns();
	categoryIndex_ = categoryIndex;
	missingValueMark_ = missingValueMark;

	// Utworzenie tablicy na przyklady
	shared_array<shared_array<size_t> > ex(
			new shared_array<size_t> [examplesCount_]);
	for (size_t i = 0; i < examplesCount_; ++i)
		ex[i] = shared_array<size_t>(new size_t[attributesCount_]);

	// Mapowanie stringow na size_t
	// oraz zapamietywanie wystepujacych wartosci atrybutow
	for (size_t i = 0; i < attributesCount_; ++i) {
		for (size_t j = 0; j < examplesCount_; ++j) {
			size_t k = 0;
			for (; k < values_[i].size(); ++k) { // sprawdzenie czy taka wartosc juz wystapila
				if (examples.getValueAt(j, i) == values_[i][k])
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
	for (size_t i = 0; i < values_.size(); ++i) {
		cout << i << ": ";
		for (size_t j = 0; j < values_[i].size(); ++j)
			cout << j << "-" << values_[i][j] << " ";
		cout << endl;
	}

	// Wypisanie tablicy na przyklady
	cout << "ex: " << endl;
	for (size_t i = 0; i < examplesCount_; ++i) {
		for (size_t j = 0; j < attributesCount_; ++j)
			cout << ex[i][j] << " ";
		cout << endl;
	}
#endif
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
