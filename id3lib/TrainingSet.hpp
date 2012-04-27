/*
 * TrainingSet.hpp
 */

#ifndef TRAININGSET_HPP_
#define TRAININGSET_HPP_

#include <string>
#include <vector>
#include "Table.hpp"

namespace id3lib {

/*
 * Klasa reprezentujaca zbior trenujacy na podstawie
 * ktorego mozna utworzyc drzewo decyzyjne
 */
class TrainingSet: public Table {
public:
	TrainingSet(const std::vector<std::string>& attr, size_t categoryIndex,
			const std::string& missingValueMark) throw (std::invalid_argument):
			Table(attr), categoryIndex_(categoryIndex), missingValueMark_(
					missingValueMark) {
		if (categoryIndex_ >= attr.size())
			throw std::invalid_argument("Category index is out of bounds.");
	}
	virtual ~TrainingSet() {

	}
	/*
	 * Dodaje przyklad na koniec kolekcji sprawdzajac
	 * czy w kolumnie kategorii nie ma pustej wartosci
	 */
	virtual void pushBack(const std::vector<std::string>& x)
			throw (std::invalid_argument);
	/*
	 * @return Indeks atrybutu bedacego kategoria
	 */
	size_t getCategoryIndex() const {
		return categoryIndex_;
	}
	/*
	 * @return Znak reprezentujacy brakujaca wartosc
	 */
	const std::string& getMissingValueMark() const {
		return missingValueMark_;
	}
private:
	/*
	 * Indeks atrybutu bedacego kategoria
	 */
	size_t categoryIndex_;
	/*
	 * Znak reprezentujacy brakujaca wartosc
	 */
	std::string missingValueMark_;
};

} /* namespace id3lib */
#endif /* TRAININGSET_HPP_ */
