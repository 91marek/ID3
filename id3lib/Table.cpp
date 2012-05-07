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

size_t Table::readFromStream(istream& is, const string& separator) {
	string line;
	size_t readed = 0;
	for (size_t i = 0; is; ++i) {
		getline(is, line);
		if (line == "")
			break;
		char_separator<char> sep(separator.c_str());
		typedef tokenizer<char_separator<char> > tokenizer;
		tokenizer tok(line, sep);
		vector<string> v = vector<string>();
		for (tokenizer::iterator tok_iter = tok.begin(); tok_iter != tok.end(); ++tok_iter) {
			v.push_back(string(*tok_iter));
		}
		try {
			pushBack(v);
			++readed;
		} catch (invalid_argument& ia) {
			cerr << "Error while reading examples from stream. Line: " << i << " What: " << ia.what() << endl;
		}
	}
	return readed;
}
