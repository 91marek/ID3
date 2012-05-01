/*
 * Table.cpp
 */

#include "Table.hpp"
#include <boost/tokenizer.hpp>
#include <iostream>
#include <fstream>

using namespace std;
using namespace boost;
using namespace id3lib;

void Table::pushBack(const vector<string>& x) throw (invalid_argument) {
	if (x.size() != columns_)
		throw invalid_argument(
				"Inserted row must have the same size as already existed rows.");
	examples_.push_back(x);
}

void Table::readFromFile(const std::string& path) throw (runtime_error, invalid_argument) {
	string line;
	ifstream file(path.c_str());
	if (file.is_open()) {
		while (file.good()) {
			getline(file, line);
			if (line == "")
				break;
			tokenizer<> tok(line);
			vector<string> v = vector<string>();
			for (tokenizer<>::iterator beg = tok.begin(); beg != tok.end(); ++beg) {
				v.push_back(string(*beg));
			}
			pushBack(v);
		}
		file.close();
	} else
		throw runtime_error("Unable to open file.");
}
