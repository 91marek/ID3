/*
 * Table.cpp
 */

#include "Table.hpp"

Table::Table(const std::vector<std::string>& attr) :
		attr_(attr), examples_() {
}

Table::~Table() {

}

void Table::pushBack(const std::vector<std::string>& x) {
	if (x.size() != columns())
		throw std::exception();
	examples_.push_back(x);
}

std::vector<std::string> Table::getAttr() const {
	return attr_;
}

unsigned Table::columns() const {
	return attr_.size();
}

unsigned Table::rows() const {
	return examples_.size();
}
