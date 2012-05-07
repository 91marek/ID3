/*
 * UMDecisionTree.cpp
 */

#ifdef DEBUG
#include <boost/assert.hpp>
const float EPSILON = 0.00001;
#endif

#include "UMDecisionTree.hpp"

using namespace id3lib;
using namespace std;
using namespace boost;

PExamplesForChildren UMDecisionTree::split(ListOfExamples examples, size_t test,
		shared_array<shared_array<int> > table) const {
	float savedExamplesWeightSum = examples.weightSum();
	// Utworzenie struktury na przyklady dla dzieci
	size_t childrenCount = values_[test].size(); // liczba dzieci
	PExamplesForChildren forChildren(new ExamplesForChildren(childrenCount));
	for (size_t i = 0; i < forChildren->size(); ++i)
		(*forChildren)[i].reset(new ListOfExamples);

	// Podzielenie przykladow ze znanymi wartosciami wybranego testu
	for (ListOfExamples::iterator i = examples.begin(); i != examples.end();) {
		int value = table[i->number][test];
		if (value == -1) // nieznana wartosc
			++i;
		else {
			(*forChildren)[static_cast<size_t>(value)]->pushBack(
					Example(i->number, i->weight));
			i = examples.erase(i);
		}
	}

	// Wyznaczenie mnoznikow
	vector<float> multipliers = vector<float>(childrenCount, float(0.0f));
	float alreadySplitted = savedExamplesWeightSum - examples.weightSum();
	for (size_t i = 0; i < multipliers.size(); ++i) {
		multipliers[i] = (*forChildren)[i]->weightSum() / alreadySplitted;
	}

	// Podzielenie przykladow z nieznanymi wartosciami wybranego testu
	// jesli sa wylacznie przyklady z nieznanymi wartosciami to nie zostana podzielone
	for (ListOfExamples::const_iterator i = examples.begin();
			i != examples.end(); ++i) {
		for (size_t j = 0; j < forChildren->size(); ++j) {
			if (0 == (*forChildren)[j]->size()) // nie bylo przykladu o tej wartosci atrybutu
				continue;
			(*forChildren)[j]->pushBack(
					Example(i->number, i->weight * multipliers[j]));
		}
	}
	return forChildren;
}

void UMDecisionTree::minimumErrorPruning(unsigned m) throw (logic_error) {
	// Sprawdzenie poprawnosci stanu obiektu
	if (NULL == root.get())
		throw logic_error("Decision tree must be built to prune.");

	recursiveMEP(root, m);
#ifdef DEBUG
	cout << "Drzewo po MEP:" << endl << *this;
#endif
}

float UMDecisionTree::recursiveMEP(PNode node, unsigned m) {
	float k = static_cast<float>(values_[categoryIndex_].size()); // liczba kategorii
	float nodeErrorRate = (node->getMisclassifiedExamplesCount()
			+ m * (k - 1) / k) / (node->getExamplesCount() + m); // MEP error rate

	if (node->isLeaf())
		return nodeErrorRate;

	float subtreeErrorRate = 0.0f;
	for (size_t i = 0; i < node->getChildrenCount(); ++i) {
		PNode child(node->getChildAt(i));
		if (child != NULL) {
			float multiplier = child->getExamplesCount()
					/ node->getExamplesCount();
			subtreeErrorRate += (multiplier * recursiveMEP(child, m));
		}
	}

	if (nodeErrorRate < subtreeErrorRate) { // warunek przyciecia
		node->makeLeaf();
		return nodeErrorRate;
	}

	return subtreeErrorRate;
}
