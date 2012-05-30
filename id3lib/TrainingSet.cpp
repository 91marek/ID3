/*
 * TrainingSet.cpp
 */

#include "TrainingSet.hpp"


using std::invalid_argument;
using std::vector;
using std::string;
using namespace id3lib;

void TrainingSet::pushBack(const vector<string>& x) throw (invalid_argument) {
	if (x.size() != columns_)
		throw invalid_argument("Inserted row must have the same size as already existed rows.");
	if (x[categoryIndex_] == missingValueMark_)
		throw invalid_argument("Category column could not have missing values.");
	examples_.push_back(x);
}
