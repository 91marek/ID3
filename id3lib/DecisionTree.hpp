/*
 * DecisionTree.hpp
 */

#ifndef DECISIONTREE_HPP_
#define DECISIONTREE_HPP_

#include "Table.hpp"
#include "Node.hpp"
#include <string>
#include <vector>

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
	 * Niszczy drzewo decyzyjne
	 */
	~DecisionTree();
	/*
	 * Buduje drzewo decyzyjne na podstawie przykladow
	 */
	void build(const Table& examples, size_t categoryIndex,
			const std::string& missingValueMark);
	/*
	 * Przycina drzewo na podstawie zbioru trenujacego
	 */
	void prune();
	/*
	 * Przycina drzewo na podstawie zadanych przykladow
	 */
	void prune(const Table& examples);
	/*
	 * Klasyfikuje zadane przyklady
	 */
	std::vector<std::string> classify(const Table& examples) const;
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
protected:
	/*
	 * Slownik wartosci atrybutow jakie pojawily sie
	 * w zbiorze trenujacym
	 */
	std::vector<std::vector<std::string> > values_;
	std::vector<std::string> attributes_;
	size_t examplesCount_;
	size_t attributesCount_;
	size_t categoryIndex_;
	std::string missingValueMark_;
	Node* root;
};

} /* namespace id3lib */
#endif /* DECISIONTREE_HPP_ */
