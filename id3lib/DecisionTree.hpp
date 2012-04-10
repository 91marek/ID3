/*
 * DecisionTree.hpp
 */

#ifndef DECISIONTREE_HPP_
#define DECISIONTREE_HPP_

#include <string>
#include <vector>

namespace id3lib {
/*
 * Klasa reprezentujaca drzewo decyzyjne
 */
class DecisionTree {
public:
	/*
	 * Tworzy drzewo decyzyjne na podstawie zbioru trenujacego
	 * @param categoryIndex Indeks atrybutu bedacego kategoria
	 */
	DecisionTree(const std::vector<std::vector<std::string> >& examples,
			const std::vector<std::string>& attributes, unsigned categoryIndex);
	~DecisionTree();
	/*
	 * Przycina drzewo na podstawie zbioru trenujacego
	 */
	void prune();
	/*
	 * Przycina drzewo na podstawie zadanych przykladow
	 */
	void prune(const std::vector<std::vector<std::string> >& examples);
	/*
	 * Klasyfikuje zadane przyklady
	 */
	std::vector<std::string> classify(
			const std::vector<std::vector<std::string> >& examples) const;
	/*
	 * @return Liczba atrybutow zbioru trenujacego
	 * na podstawie ktorego drzewo zostalo utworzone
	 */
	unsigned getAttributesCount() const;
	/*
	 * @return Liczba przykladow zbioru trenujacego
	 * na podstawie ktorego drzewo zostalo utworzone
	 */
	unsigned getExamplesCount() const;
	/*
	 * @return Indeks atrybutu bedacego kategoria
	 */
	unsigned getCategoryIndex() const;
protected:
	/*
	 * Wartosci poszczegolnych atrybutow jakie pojawily sie
	 * w zbiorze trenujacym
	 */
	std::vector<std::vector<std::string> > values_;
	const std::vector<std::string> attributes_;
	const unsigned examplesCount_;
	const unsigned attributesCount_;
	const unsigned categoryIndex_;
};
}

#endif /* DECISIONTREE_HPP_ */
