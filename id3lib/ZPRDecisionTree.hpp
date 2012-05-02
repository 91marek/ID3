/*
 * ZPRDecisionTree.hpp
 */

#ifndef ZPRDECISIONTREE_HPP_
#define ZPRDECISIONTREE_HPP_

#include "DecisionTree.hpp"
#include "ErrorRate.hpp"

namespace id3lib {

class ZPRDecisionTree : public DecisionTree {
public:
	/*
	 * Tworzy puste drzewo decyzyjne
	 */
	ZPRDecisionTree() : DecisionTree() {

	}
	/*
	 * Przycina drzewo metoda Reduced Error Pruning
	 */
	void reducedErrorPruning(const TrainingSet& examples) throw (std::logic_error);
protected:
	/*
	 * Rekurencyjna funkcja realizujaca przycinanie
	 * metoda REP
	 */
	ErrorRate recursiveREP(PNode node,
			boost::shared_array<boost::shared_array<int> > t, size_t rows);
	/*
	 * Dzieli przyklady wg zadanego testu
	 * Dla brakujacych wartosci stosowana jest
	 * wartosc najczesciej wystepujaca
	 * w aktualnym zbiorze przykladow
	 */
	virtual PExamplesForChildren split(ListOfExamples examples, size_t test,
			boost::shared_array<boost::shared_array<int> > table) const;
};

} /* namespace id3lib */
#endif /* ZPRDECISIONTREE_HPP_ */
