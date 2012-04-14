/*
 * Table.cpp
 */

#include "Table.hpp"

using namespace std;
using namespace id3lib;

void Table::pushBack(const vector<string>& x) throw (invalid_argument) {
	if (x.size() != columns_)
		throw invalid_argument(
				"Inserted row must have the same size as already existed rows.");
	examples_.push_back(x);
}
