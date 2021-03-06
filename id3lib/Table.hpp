/*
 * Table.hpp
 */

#ifndef TABLE_HPP_
#define TABLE_HPP_

#include <string>
#include <vector>
#include <stdexcept>

#ifdef _MSC_VER
#pragma warning( disable : 4290 )
#endif

namespace id3lib {

/*
 * Klasa przechowujaca przyklady. Nazwy atrybutow sa inicjowane
 * w konstruktorze i sa stale dla obiektu.
 */
class Table {
public:
	/*
	 * Tworzy pusta tabele o okreslonych atrybutach
	 */
	Table(const std::vector<std::string>& attr) throw (std::invalid_argument) :
			attr_(attr), examples_(), columns_(attr.size()) {
		if (attr.size() < 2)
			throw std::invalid_argument("Table must have 2 or more columns.");
	}
	virtual ~Table() {

	}
	/*
	 * Wczytuje przyklady z pliku i dodaje je na koniec kolekcji
	 * @return Liczba wczytanych przykladow
	 */
	size_t readFromStream(std::istream& is, const std::string& separator);
	/*
	 * Dodaje przyklad na koniec kolekcji
	 */
	virtual void pushBack(const std::vector<std::string>& x)
			throw (std::invalid_argument);
	/*
	 * @return Nazwa atrybutu o podanym indeksie
	 */
	const std::string& getAttrAt(size_t i) const {
		return attr_.at(i);
	}
	/*
	 * @return Kolekcja nazw atrybutow
	 */
	const std::vector<std::string>& getAttr() const {
		return attr_;
	}
	/*
	 * @return Wartosc okreslonego atrybutu okreslonego przykladu
	 */
	const std::string& getValueAt(size_t x, size_t y) const {
		return examples_[x][y];
	}
	/*
	 * @return Liczba kolumn
	 */
	size_t columns() const {
		return columns_;
	}
	/*
	 * @return Liczba wierszy
	 */
	size_t rows() const {
		return examples_.size();
	}
protected:
	/*
	 * Nazwy atrybutow
	 */
	const std::vector<std::string> attr_;
	/*
	 * Przyklady
	 */
	std::vector<std::vector<std::string> > examples_;
	/*
	 * Liczba kolumn
	 */
	const size_t columns_;
};

} /* namespace id3lib */
#endif /* TABLE_HPP_ */
