/*
 * ZPRDecisionTree.cpp
 */

#ifdef DEBUG
#include <boost/assert.hpp>
const float EPSILON = 0.00001;
#endif

#include "ZPRDecisionTree.hpp"

using namespace id3lib;
using namespace std;
using namespace boost;

PExamplesForChildren ZPRDecisionTree::split(ListOfExamples examples, size_t test,
		shared_array<shared_array<int> > table) const {
	// Utworzenie struktury na przyklady dla dzieci
	size_t childrenCount = values_[test].size(); // liczba dzieci
	PExamplesForChildren forChildren(new ExamplesForChildren(childrenCount));
	for (size_t i = 0; i < forChildren->size(); ++i)
		(*forChildren)[i].reset(new ListOfExamples);

	// Wektor do wyznaczenia najczesciej wystepujacej wartosci
	vector<float> childrenExamplesCounter = vector<float>(childrenCount, 0.0f);

	// Podzielenie przykladow ze znanymi wartosciami wybranego testu
	for (ListOfExamples::iterator i = examples.begin(); i != examples.end();) {
		int value = table[i->number][test];
		if (value == -1) // nieznana wartosc
			++i;
		else {
			size_t index = static_cast<size_t>(value);
			(*forChildren)[index]->pushBack(Example(i->number, i->weight));
			childrenExamplesCounter[index] += i->weight;
			i = examples.erase(i);
		}
	}

	// Wyznaczenie najczesciej wystepujacej wartosci
	float max = 0.0f;
	size_t maxIndex = 0;
	for (size_t i = 0; i < childrenExamplesCounter.size(); ++i) {
		if (childrenExamplesCounter[i] > max) {
			max = childrenExamplesCounter[i];
			maxIndex = i;
		}
	}

	// Jesli sa wylacznie przyklady z nieznanymi wartosciami to nie zostana podzielone
	if (max == 0.0f)
		return forChildren;

	// Podzielenie przykladow z nieznanymi wartosciami wybranego testu
	for (ListOfExamples::const_iterator i = examples.begin();
			i != examples.end(); ++i) {
		(*forChildren)[maxIndex]->pushBack(Example(i->number, i->weight));
	}

	return forChildren;
}

void ZPRDecisionTree::reducedErrorPruning(const TrainingSet& examples) throw (logic_error) {
	// Sprawdzenie poprawnosci stanu obiektu
	if (NULL == root.get())
		throw logic_error("Decision tree must be built to prune.");
	// Sprawdzenie poprawnosci parametru
	if (0 == examples.rows()) // brak przykladow
		throw invalid_argument("Table must have 1 or more rows (1 or more examples).");
	if (examples.columns() != attributesCount_)
		throw invalid_argument("Examples must have the same number of attributes as decision tree.");
	for (size_t i = 0; i < examples.columns(); ++i)
		if (!(attributes_[i] == examples.getAttrAt(i)))
			throw invalid_argument("Examples must have the same names of attributes as decision tree.");

	// Mapowanie przykladow na tabele int-ow wg zapamietanego slownika
	shared_array<shared_array<int> > table = map(examples);

	recursiveREP(root, table, examples.rows());
#ifdef DEBUG
	cout << "Drzewo po REP:" << endl << *this;
#endif
}

ErrorRate ZPRDecisionTree::recursiveREP(PNode node, shared_array<shared_array<int> > t, size_t rows) {
	// obliczenie bledu na zbiorze do przycinania
	ErrorRate leafErrorRate;
	int category = static_cast<int>(node->getCategory());
	for (size_t i = 0; i < rows; ++i) {
		if (t[i][categoryIndex_] != category)
			leafErrorRate.misclassifiedExample();
		else
			leafErrorRate.correctlyClassifiedExample();
	}

	if (node->isLeaf())
		return leafErrorRate;

	ErrorRate treeErrorRate;
	for (size_t i = 0; i < node->getChildrenCount(); ++i) {
		PNode child(node->getChildAt(i));
		if (child != NULL) {
			treeErrorRate += recursiveREP(child, t, rows);
		}
	}

	if (leafErrorRate < treeErrorRate) {	// warunek przyciecia
		node->makeLeaf();
		return leafErrorRate;
	}

	return treeErrorRate;
}
