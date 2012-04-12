/*
 * Table.cpp
 */

#include "Table.hpp"

using namespace std;
using namespace id3lib;

void Table::pushBack(const vector<string>& x) throw(BadSizeException) {
	if (x.size() != columns_)
		throw BadSizeException();
	examples_.push_back(x);
}
