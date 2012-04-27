/*
 * DecisionTree.hpp
 */

#ifndef DECISIONTREE_HPP_
#define DECISIONTREE_HPP_

#include "Table.hpp"
#include "TrainingSet.hpp"
#include "Node.hpp"
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>
#include <stdexcept>

namespace id3lib {

/*
 * Klasa reprezentujaca drzewo decyzyjne
 */
class DecisionTree {
public:
	/*
	 * Tworzy puste drzewo decyzyjne
	 */
	DecisionTree();
	/*
	 * Buduje drzewo decyzyjne na podstawie przykladow
	 */
	void build(const TrainingSet& examples) throw (std::invalid_argument);
	/*
	 * Przycina drzewo metoda Minimum Error Prunning
	 */
	void minimumErrorPrunning(unsigned m) throw (std::logic_error);
	/*
	 * Przycina drzewo metoda Reduced Error Prunning
	 */
	void reducedErrorPrunning(const Table& examples) throw (std::logic_error);
	/*
	 * Klasyfikuje zadane przyklady
	 */
	boost::shared_ptr<std::vector<std::string> > classify(const Table& examples) const
			throw (std::logic_error, std::invalid_argument);
	/*
	 * @return Nazwy atrybutow zbioru trenujacego
	 * na podstawie ktorego drzewo zostalo utworzone
	 */
	const std::vector<std::string>& getAttributes() const {
		return attributes_;
	}
	/*
	 * @return Liczba atrybutow zbioru trenujacego
	 * na podstawie ktorego drzewo zostalo utworzone
	 */
	size_t getAttributesCount() const {
		return attributesCount_;
	}
	/*
	 * @return Liczba przykladow zbioru trenujacego
	 * na podstawie ktorego drzewo zostalo utworzone
	 */
	size_t getExamplesCount() const {
		return examplesCount_;
	}
	/*
	 * @return Indeks atrybutu bedacego kategoria
	 */
	size_t getCategoryIndex() const {
		return categoryIndex_;
	}
	/*
	 * @return Liczba kategorii
	 */
	size_t getCategoryCount() const {
		return values_[categoryIndex_].size();
	}
	/*
	 * @return Znak reprezentujacy brakujaca wartosc
	 */
	const std::string& getMissingValueMark() const {
		return missingValueMark_;
	}
	/*
	 * Wypisuje drzewo
	 */
	std::ostream& operator<<(std::ostream& os) const;
protected:
	/*
	 * Slownik wartosci atrybutow jakie pojawily sie
	 * w zbiorze trenujacym
	 */
	std::vector<std::vector<std::string> > values_;
	/*
	 * Atrybuty przykladow na podstawie ktorych
	 * drzewo zostalo utworzone
	 */
	std::vector<std::string> attributes_;
	/*
	 * Liczba przykladow na podstawie ktorych
	 * drzewo zostalo utworzone
	 */
	size_t examplesCount_;
	/*
	 * Liczba atrybutow na podstawie ktorych
	 * drzewo zostalo utworzone
	 */
	size_t attributesCount_;
	/*
	 * Indeks atrybutu bedacego kategoria
	 */
	size_t categoryIndex_;
	/*
	 * Znak reprezentujacy brakujaca wartosc
	 */
	std::string missingValueMark_;
	/*
	 * Wskazanie na korzen drzewa decyzyjnego
	 */
	PNode root;
	/*
	 * Rekurencyjna funkcja realizujaca przycinanie
	 * metoda MEP
	 */
	float recursiveMEP(PNode node, unsigned m);
	/*
	 * Rekurencyjne wypisywanie drzewa
	 */
	void recursivePrintTree(std::ostream& os, PNode node, size_t depth) const;
};

} /* namespace id3lib */
#endif /* DECISIONTREE_HPP_ */
