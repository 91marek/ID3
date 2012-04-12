/*
 * Table.hpp
 */

#ifndef TABLE_HPP_
#define TABLE_HPP_

#include <string>
#include <vector>
#include <exception>

/*
 * Klasa przechowujaca przyklady. Atrybuty sa inicjowane
 * w konstruktorze i sa stale dla obiektu.
 */
class Table {
private:
	const std::vector<std::string> attr_;
	std::vector<std::vector<std::string> > examples_;
public:

	Table(const std::vector<std::string>& attr);
	Table(const std::vector<std::string>& attr, unsigned rows);
	~Table();

	void pushBack(const std::vector<std::string>& x);
	std::vector<std::string> getAttr() const;
	unsigned columns() const;
	unsigned rows() const;
};

#endif /* TABLE_HPP_ */
