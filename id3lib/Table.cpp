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

void Table::readFromFile(const std::string& path) throw (runtime_error) {
	string line;
	ifstream file(path.c_str());
	if (file.is_open()) {
		for (size_t i = 0; file.good(); ++i) {
			getline(file, line);
			if (line == "")
				break;
			char_separator<char> sep(", ");
			typedef tokenizer<char_separator<char> > tokenizer;
			tokenizer tok(line, sep);
			vector<string> v = vector<string>();
			for (tokenizer::iterator tok_iter = tok.begin(); tok_iter != tok.end(); ++tok_iter) {
				v.push_back(string(*tok_iter));
			}
			try {
				pushBack(v);
			} catch (invalid_argument& ia) {
				cerr << "File: " << path << " Line: " << i << " What: " << ia.what() << endl;
			}
		}
		file.close();
	} else
		throw runtime_error("Unable to open file: " + path);
}
