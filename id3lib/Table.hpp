/*
 * Table.hpp
 */

#ifndef TABLE_HPP_
#define TABLE_HPP_

#include "BadSizeException.hpp"
#include <string>
#include <vector>

namespace id3lib {

/*
 * Klasa przechowujaca przyklady. Atrybuty sa inicjowane
 * w konstruktorze i sa stale dla obiektu.
 */
class Table {
public:
	Table(const std::vector<std::string>& attr) :
			attr_(attr), examples_() , columns_(attr.size()) {
	}

	void pushBack(const std::vector<std::string>& x) throw(BadSizeException);

	const std::string& getAttrAt(size_t i) const {
		return attr_.at(i);
	}

	const std::vector<std::string>& getAttr() const {
		return attr_;
	}

	const std::string& getValueAt(size_t x, size_t y) const {
		return examples_[x][y];
	}

	size_t columns() const {
		return columns_;
	}

	size_t rows() const {
		return examples_.size();
	}
protected:
	const std::vector<std::string> attr_;
	std::vector<std::vector<std::string> > examples_;
	const size_t columns_;
};

} /* namespace id3lib */
#endif /* TABLE_HPP_ */
