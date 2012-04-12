/*
 * Table.cpp
 */

#include "Table.hpp"
#include "BadSizeException.hpp"

using namespace std;
using namespace id3lib;

void Table::pushBack(const vector<string>& x) {
	if (x.size() != columns())
		throw BadSizeException();
	examples_.push_back(x);
}
