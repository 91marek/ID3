/*
 * UMDecisionTree.hpp
 */

#ifndef UMDECISIONTREE_HPP_
#define UMDECISIONTREE_HPP_

#include "ZPRDecisionTree.hpp"

namespace id3lib {

class UMDecisionTree: public ZPRDecisionTree {
public:
	/*
	 * Tworzy puste drzewo decyzyjne
	 */
	UMDecisionTree() :
			ZPRDecisionTree() {

	}
	/*
	 * Przycina drzewo metoda Minimum Error Pruning
	 */
	void minimumErrorPruning(unsigned m) throw (std::logic_error);
protected:
	/*
	 * Rekurencyjna funkcja realizujaca przycinanie
	 * metoda MEP
	 */
	float recursiveMEP(PNode node, unsigned m);
	/*
	 * Dzieli przyklady wg zadanego testu
	 * Dla brakujacych wartosci stosowana jest
	 * technika przykladow ulamkowych
	 */
	virtual PExamplesForChildren split(ListOfExamples examples, size_t test,
			boost::shared_array<boost::shared_array<int> > table) const;
};

} /* namespace id3lib */
#endif /* UMDECISIONTREE_HPP_ */
