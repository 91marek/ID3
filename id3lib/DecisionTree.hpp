/*
 * DecisionTree.hpp
 */

#ifndef DECISIONTREE_HPP_
#define DECISIONTREE_HPP_

#include "Table.hpp"
#include "TrainingSet.hpp"
#include "Node.hpp"
#include "ErrorRate.hpp"
#include <boost/shared_array.hpp>
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
	 * Przycina drzewo metoda Minimum Error Pruning
	 */
	void minimumErrorPruning(unsigned m) throw (std::logic_error);
	/*
	 * Przycina drzewo metoda Reduced Error Pruning
	 */
	void reducedErrorPruning(const TrainingSet& examples) throw (std::logic_error);
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
	 * Rekurencyjna funkcja realizujaca przycinanie
	 * metoda REP
	 */
	ErrorRate recursiveREP(PNode node, boost::shared_array<boost::shared_array<int> > t, size_t columns);
	/*
	 * Rekurencyjne wypisywanie drzewa
	 */
	void recursivePrintTree(std::ostream& os, PNode node, size_t depth, size_t test, size_t result) const;
	/*
	 * Tworzy tabele int-ow o zadanym rozmiarze
	 */
	boost::shared_array<boost::shared_array<int> > makeTable(size_t rows, size_t columns) const;
	/*
	 * Mapuje przyklady na tabele int-ow wg zapamietanego slownika
	 */
	boost::shared_array<boost::shared_array<int> > map(const Table& examples) const;
	/*
	 * Wypisuje drzewo
	 */
	friend std::ostream& operator<<(std::ostream& os, const DecisionTree& dt);
};

std::ostream& operator<<(std::ostream& os, const DecisionTree& dt);

} /* namespace id3lib */

#endif /* DECISIONTREE_HPP_ */
