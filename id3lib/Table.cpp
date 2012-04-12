/*
 * Table.cpp
 */

#include "Table.hpp"

using namespace std;

Table::Table(const vector<string>& attr) :
		attr_(attr), examples_() {
}

Table::Table(const vector<string>& attr, unsigned rows) :
		attr_(attr), examples_(rows) {
}

Table::~Table() {

}

void Table::pushBack(const vector<string>& x) {
	if (x.size() != columns())
		throw exception();
	examples_.push_back(x);
}

vector<string> Table::getAttr() const {
	return attr_;
}

unsigned Table::columns() const {
	return attr_.size();
}

unsigned Table::rows() const {
	return examples_.size();
}
