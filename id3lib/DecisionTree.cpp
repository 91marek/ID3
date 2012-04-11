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

DecisionTree::DecisionTree(const vector<vector<string> >& examples,
		const vector<string>& attributes, unsigned categoryIndex) :
		values_(vector<vector<string> >(attributes.size())), attributes_(
				attributes), examplesCount_(examples.size()), attributesCount_(
				attributes.size()), categoryIndex_(categoryIndex) {

	// Utworzenie tablicy na przyklady
	boost::shared_array< boost::shared_array<unsigned> >  ex(new boost::shared_array<unsigned>[examplesCount_]);
	for (unsigned i = 0; i < examplesCount_; ++i) {
		boost::shared_array<unsigned> tmp(new unsigned[attributesCount_]);
		ex[i] = tmp;
	}

	// Mapowanie stringow na unsigned
	// oraz zapamietywanie wystepujacych wartosci atrybutow
	for (unsigned i = 0; i < attributesCount_; ++i) {
		for (unsigned j = 0; j < examplesCount_; ++j) {
			unsigned k = 0;
			for (; k < values_[i].size(); ++k) { // sprawdzenie czy taka wartosc juz wystapila
				if (0 == examples[j][i].compare(values_[i][k]))
					break;
			}
			if (k < values_[i].size()) // taka wartosc juz wystapila
				ex[j][i] = k;
			else { // zapamietanie nowej wartosci
				values_[i].push_back(examples[j][i]);
				ex[j][i] = values_[i].size() - 1;
			}
		}
	}

#ifdef DEBUG
	// Wypisanie wystepujacych wartosci atrybutow
	cout << "values_:" << endl;
	for (unsigned i = 0; i < values_.size(); ++i)
		for (unsigned j = 0; j < values_[i].size(); ++j)
			cout << "i= " << i << " j=" << j << " " << values_[i][j] << endl;

	// Wypisanie tablicy na przyklady
	cout << "ex: " << endl;
	for (unsigned i = 0; i < examplesCount_; ++i)
		for (unsigned j = 0; j < attributesCount_; ++j)
			cout << "i=" << i << " j=" << j << " " << ex[i][j] << endl;
#endif
/* niepotrzebne
	// Destrukcja tablicy na przyklady
	for (unsigned i = 0; i < examplesCount_; ++i)
		delete[] ex[i];
	delete[] ex;
*/
}

DecisionTree::~DecisionTree() {

}

void DecisionTree::prune() {
	// TODO
	return;
}

void DecisionTree::prune(const vector<vector<string> >& examples) {
	// TODO
	return;
}

vector<string> DecisionTree::classify(const vector<vector<string> >& examples) const {
	// TODO
	return vector<string>();
}

inline unsigned DecisionTree::getAttributesCount() const {
	return attributesCount_;
}

inline unsigned DecisionTree::getExamplesCount() const {
	return examplesCount_;
}

inline unsigned DecisionTree::getCategoryIndex() const {
	return categoryIndex_;
}
