/*
 * DecisionTree.cpp
 */

#define DEBUG

#ifdef DEBUG
#include <iostream>
#include <boost/assert.hpp>
#endif

#include "DecisionTree.hpp"
#include "Example.hpp"
#include "ListOfExamples.hpp"
#include "UnvisitedNode.hpp"
#include <boost/shared_array.hpp>
#include <queue>

using namespace id3lib;
using namespace std;
using namespace boost;

#ifdef DEBUG
void printTree(Node* node, size_t depth) {
	for (size_t i = 0; i < depth; ++i)
		cout << "*******";
	cout << "t:" << node->getTest() << " c:" << node->getCategory() << endl;
	size_t new_depth = ++depth;
	for (size_t i = 0; i < node->getChildrenCount(); ++i) {
		Node* child = node->getChildAt(i);
		if (child != NULL)
			printTree(child, new_depth);
	}
}
#endif

DecisionTree::DecisionTree() :
		values_(vector<vector<string> >()), attributes_(vector<string>()), examplesCount_(
				0), attributesCount_(0), categoryIndex_(0), missingValueMark_(
				""), root(NULL) {

}

DecisionTree::~DecisionTree() {
	delete root;
}

void DecisionTree::build(const Table& examples, size_t categoryIndex,
		const string& missingValueMark) throw (invalid_argument) {
	// Sprawdzenie poprawnosci parametrow
	if (examples.columns() < 2) // musi byc przynajmniej 1 atrybut + kategoria
		throw invalid_argument(
				"Table must have 2 or more columns (1 or more attributes + category).");
	if (0 == examples.rows()) // brak przykladow w zbiorze trenujacym
		throw invalid_argument(
				"Table must have 1 or more rows (1 or more examples).");
	if (categoryIndex >= examples.columns()) // nie ma tylu kolumn w zbiorze przykladow
		throw out_of_range("Index of category column is out of bounds.");
	// jezeli to sprawdzenie zostanie wykonane pozniej to obiekt moze pozostac
	// w stanie nieustalonym na skutek rzucenia wyjatku
	// (degeneracja dotychczas zbudowanego drzewa == bledy przy klasyfikacji)
	for (size_t x = 0; x < examples.rows(); ++x)
		if (examples.getValueAt(x, categoryIndex) == missingValueMark) // przyklad z brakujaca kategoria
			throw invalid_argument("Category could not have missing values.");

	// Inicjalizacja
	values_ = vector<vector<string> >(examples.columns());
	attributes_ = examples.getAttr();
	examplesCount_ = examples.rows();
	attributesCount_ = examples.columns();
	categoryIndex_ = categoryIndex;
	missingValueMark_ = missingValueMark;

	// Utworzenie tablicy na przyklady
	shared_array<shared_array<int> > table(
			new shared_array<int> [examplesCount_]);
	for (size_t i = 0; i < examplesCount_; ++i)
		table[i] = shared_array<int>(new int[attributesCount_]);

	// Mapowanie stringow na int
	// oraz zapamietywanie wystepujacych wartosci atrybutow
	for (size_t y = 0; y < attributesCount_; ++y) {
		for (size_t x = 0; x < examplesCount_; ++x) {
			if (examples.getValueAt(x, y) == missingValueMark_) {
				table[x][y] = -1; // -1 dla brakujacych wartosci
				continue;
			}
			size_t k = 0;
			for (; k < values_[y].size(); ++k) { // sprawdzenie czy taka wartosc juz wystapila
				if (examples.getValueAt(x, y) == values_[y][k])
					break;
			}
			if (k < values_[y].size()) // taka wartosc juz wystapila
				table[x][y] = static_cast<int>(k);
			else { // zapamietanie nowej wartosci
				values_[y].push_back(examples.getValueAt(x, y));
				table[x][y] = static_cast<int>(values_[y].size()) - 1;
			}
		}
	}

#ifdef DEBUG
	// Wypisanie wystepujacych wartosci atrybutow
	cout << "values_:" << endl;
	for (size_t i = 0; i < values_.size(); ++i) {
		cout << i << ": ";
		for (size_t j = 0; j < values_[i].size(); ++j)
			cout << j << "-" << values_[i][j] << " ";
		cout << endl;
	}

	// Wypisanie tablicy na przyklady
	cout << "table: " << endl;
	for (size_t i = 0; i < examplesCount_; ++i) {
		for (size_t j = 0; j < attributesCount_; ++j)
			cout << table[i][j] << " ";
		cout << endl;
	}
#endif

	// Utworzenie listy przykladow zwiazanych z wezlem root
	ListOfExamples* e = new ListOfExamples();
	for (size_t i = 0; i < examplesCount_; ++i)
		e->pushBack(Example(i, 1.0f));

#ifdef DEBUG
	BOOST_ASSERT(e->size() == examplesCount_);
	BOOST_ASSERT(e->weightSum() == static_cast<float>(examplesCount_));
	for (ListOfExamples::const_iterator iter = e->begin(); iter != e->end();
			++iter)
		BOOST_ASSERT(iter->weight == 1.0f);
#endif

	// Utworzenie listy dotychczas niewykorzystanych testow
	ListOfTests* t = new ListOfTests();
	for (size_t i = 0; i < attributesCount_; ++i)
		if (i != categoryIndex_) // nie testujemy kategorii
			t->push_back(i);

#ifdef DEBUG
	BOOST_ASSERT(t->size() == attributesCount_ - 1);
#endif

	// Usuniecie biezacego drzewa
	delete root;
	// Utworzenie wezla root
	root = new Node(e->weightSum());

	// Dodanie do kolejki nieodwiedzonych wezlow wezla root
	queue<UnvisitedNode> q = queue<UnvisitedNode>();
	q.push(UnvisitedNode(root, e, t));

#ifdef DEBUG
	BOOST_ASSERT(q.size() == 1);
#endif

	// Przetwarzanie drzewa wszerz
	while (!q.empty()) {
		UnvisitedNode queueHead = q.front();
		q.pop();
		// Zliczenie sumy wag przykladow dla poszczegolnych kategorii
		vector<float> categories = vector<float>(values_[categoryIndex_].size(),
				0.0f);
		for (ListOfExamples::const_iterator iter = queueHead.examples->begin();
				iter != queueHead.examples->end(); ++iter) {
#ifdef DEBUG
			BOOST_ASSERT(table[iter->number][categoryIndex_] >= 0);
#endif
			size_t index =
					static_cast<size_t>(table[iter->number][categoryIndex_]);
			categories[index] += iter->weight;
		}
		// Wybranie kategorii wiekszosciowej
		float max = 0.0f;
		size_t maxIndex = 0;
		for (size_t i = 0; i < categories.size(); ++i) {
			if (categories[i] > max) {
				max = categories[i];
				maxIndex = i;
			}
		}
#ifdef DEBUG
		BOOST_ASSERT(max != 0.0f);
#endif
		// Zapisanie kategorii wiekszosciowej w wezle
		queueHead.node->setCategory(maxIndex);
		// Obliczenie liczby przykladow blednie klasyfikowanych
		// przez kategorie wiekoszosciowa
		float misclassifiedCounter = 0.0f;
		for (ListOfExamples::const_iterator iter = queueHead.examples->begin();
				iter != queueHead.examples->end(); ++iter) {
#ifdef DEBUG
			BOOST_ASSERT(table[iter->number][categoryIndex_] >= 0);
#endif
			if (static_cast<size_t>(table[iter->number][categoryIndex_])
					!= queueHead.node->getCategory())
				misclassifiedCounter += iter->weight;
		}
		// Zapisanie liczby przykladow blednie klasyfikowanych
		queueHead.node->setMisclassifiedExamplesCount(misclassifiedCounter);
		// Sprawdzenie kryterium stopu
		if (0.0f == queueHead.node->getMisclassifiedExamplesCount() ||	// wszystkie przyklady jednej kategorii
				0 == queueHead.tests->size()) {	// lub koniec testow
			delete queueHead.examples; // usuniecie przykladow zwiazanych z wezlem
			delete queueHead.tests; // usuniecie niewykorzystanych testow
			continue;
		}
		// Wybranie testu
		size_t bestTest = 0;
		double maxInformationGain = 0.0;
		ListOfTests::iterator bestTestIterator = queueHead.tests->begin();
#ifdef DEBUG
		size_t testCount = queueHead.tests->size();
#endif
		for (ListOfTests::iterator iter = queueHead.tests->begin();
				iter != queueHead.tests->end(); ++iter) {
			double informationGain = 0.0;
			// TODO obliczyc informationGain dla testu wskazywanego przez iter
			// tymczasowo:
			informationGain = *iter; // pozniej to wywalic!
			if (informationGain > maxInformationGain) {
				maxInformationGain = informationGain;
				bestTest = *iter;
				bestTestIterator = iter;
			}
		}
		queueHead.tests->erase(bestTestIterator);

#ifdef DEBUG
		BOOST_ASSERT(testCount - 1 == queueHead.tests->size());
		cout << "Wybrany test: " << attributes_[bestTest] << endl;
#endif

		// Zapisanie wybranego testu w wezle
		queueHead.node->setTest(bestTest);
		// Utworzenie struktury na przyklady dla dzieci
		size_t childrenCount = values_[queueHead.node->getTest()].size(); // liczba dzieci
		vector<ListOfExamples*> childrenExamples = vector<ListOfExamples*>(
				childrenCount);
		for (size_t i = 0; i < childrenExamples.size(); ++i)
			childrenExamples[i] = new ListOfExamples();
		// Bylo tak:
		// vector<ListOfExamples*> childrenExamples = vector<ListOfExamples*>(childrenCount, new ListOfExamples());
		// i nie dzialalo!!! Tworzona byla jedna lista a wskazanie do niej bylo kopiowane childrenCount razy!!!
#ifdef DEBUG
		BOOST_ASSERT(childrenExamples.size() == childrenCount);
		for (size_t i = 0; i < childrenExamples.size(); ++i) {
			BOOST_ASSERT(childrenExamples[i]->size() == 0);
		}
#endif
		// Podzielenie przykladow ze znanymi wartosciami wybranego testu
		for (ListOfExamples::iterator iter = queueHead.examples->begin();
				iter != queueHead.examples->end();) {
			int value = table[iter->number][queueHead.node->getTest()];
			if (value == -1) // nieznana wartosc
				++iter;
			else {
				childrenExamples[static_cast<size_t>(value)]->pushBack(
						Example(iter->number, iter->weight));
				iter = queueHead.examples->erase(iter);
			}
		}
		// TODO a co jesli nie bedzie przykladow ze znanymi wartosciami wybranego testu?
		// Wyznaczenie mnoznikow
		vector<float> multipliers = vector<float>(childrenCount, float(0.0f));
		for (size_t i = 0; i < multipliers.size(); ++i) {
			multipliers[i] = childrenExamples[i]->weightSum()
					/ queueHead.examples->weightSum();
		}
		// Podzielenie przykladow z nieznanymi wartosciami wybranego testu
		for (ListOfExamples::const_iterator iter = queueHead.examples->begin();
				iter != queueHead.examples->end(); ++iter) {
			for (size_t i = 0; i < childrenExamples.size(); ++i) {
				if (0.0f == multipliers[i]) // nie bylo przykladu o tej wartosci atrybutu
					continue;
				childrenExamples[i]->pushBack(
						Example(iter->number, iter->weight * multipliers[i]));
			}
		}

		// Dodanie do kolejki wszystkich dzieci z ich przykladami
		vector<Node*> children = vector<Node*>(childrenCount, NULL);
		for (size_t i = 0; i < childrenExamples.size(); ++i) {
			if (0 == childrenExamples[i]->size()) { // nie bylo przykladu o tej wartosci atrybutu
				delete childrenExamples[i];
				continue;
			}
			children[i] = new Node(childrenExamples[i]->weightSum());
			ListOfTests* childrenTests = new ListOfTests();
			*childrenTests = *(queueHead.tests);	// gleboka kopia
#ifdef DEBUG
			BOOST_ASSERT(childrenTests != queueHead.tests);	// porownanie wskaznikow
#endif
			q.push(UnvisitedNode(children[i], childrenExamples[i], childrenTests));
		}
		// Ustawienie rodzicowi wskazan na dzieci
		queueHead.node->setChildren(children);
		delete queueHead.examples; // usuniecie przykladow zwiazanych z wezlem
		delete queueHead.tests; // usuniecie niewykorzystanych testow
	}

#ifdef DEBUG
	cout << "Drzewo: " << endl;
	printTree(root, 0);
#endif
}

void DecisionTree::prune() {
	// TODO
	// pamietac aby zamieniajac wezel na lisc podmieniac vector wskazan
	// na dzieci na pusty vector (patrz metoda Node::isLeaf())
	return;
}

void DecisionTree::prune(const Table& examples) {
	// TODO
	// pamietac aby zamieniajac wezel na lisc podmieniac vector wskazan
	// na dzieci na pusty vector (patrz metoda Node::isLeaf())
	return;
}

vector<string> DecisionTree::classify(const Table& examples) const {
	// Sprawdzenie poprawnosci stanu obiektu
	if (NULL == root)
		throw logic_error("Classifier is not built.");
	// Sprawdzenie poprawnosci parametru
	if (examples.columns() != attributesCount_)
		throw invalid_argument(
				"Classifier has different number of attributes than examples to classify.");
	for (size_t i = 0; i < examples.columns(); ++i)
		if (!(attributes_[i] == examples.getAttrAt(i)))
			throw invalid_argument(
					"Classifier has different names of attributes than examples to classify.");

	// Utworzenie tablicy na przyklady
	shared_array<shared_array<int> > table(
			new shared_array<int> [examples.rows()]);
	for (size_t i = 0; i < examples.rows(); ++i)
		table[i] = shared_array<int>(new int[examples.columns()]);

	// Mapowanie stringow na int wg zapamietanego slownika
	for (size_t y = 0; y < examples.columns(); ++y) {
		for (size_t x = 0; x < examples.rows(); ++x) {
			if (examples.getValueAt(x, y) == missingValueMark_) {
				table[x][y] = -1; // -1 dla brakujacych wartosci
				continue;
			}
			size_t k = 0;
			for (; k < values_[y].size(); ++k) { // wyszukanie wartosci w slowniku
				if (examples.getValueAt(x, y) == values_[y][k])
					break;
			}
			if (k < values_[y].size()) // znaleziono wartosc w slowniku
				table[x][y] = static_cast<int>(k);
			else {	// nie znaleziono wartosci w slowniku
				table[x][y] = -1; // zakwalifikowanie do brakujacych wartosci
			}
		}
	}

	// Utworzenie listy przykladow zwiazanych z wezlem root
	ListOfExamples* e = new ListOfExamples();
	for (size_t i = 0; i < examples.rows(); ++i)
		e->pushBack(Example(i, 1.0f));

	// Utworzenie struktury przechowujacej informacje
	// o prawdopodobienstwie przynaleznosci przykladu
	// do poszczegolnych kategorii
	vector<vector<float> > probability = vector<vector<float> >();
	size_t categoryCount = values_[categoryIndex_].size();	//liczba kategorii
	for (size_t i = 0; i < examples.rows(); ++i)
		probability.push_back(vector<float>(categoryCount, 0.0f));

	// Dodanie do kolejki nieodwiedzonych wezlow wezla root
	queue<UnvisitedNode> q = queue<UnvisitedNode>();
	q.push(UnvisitedNode(root, e));

	// Przetwarzanie drzewa wszerz
	while (!q.empty()) {
		UnvisitedNode queueHead = q.front();
		q.pop();
		if (queueHead.node->isLeaf()) {	// odwiedzony wezel jest lisciem
			float all = queueHead.node->getExamplesCount();
			float incorrect = queueHead.node->getMisclassifiedExamplesCount();
			// Obliczenie prawdopodobienstwa ze kategoria przykladu zaliczonego do tego liscia
			// jest kategoria wyznaczona dla tego liscia
			float multiplier = (all - incorrect) / all;
			size_t leafCategory = queueHead.node->getCategory();	// kategoria liscia
			// Zwiekszenie prawdopodobienstwa dla wszystkich przykladow zwiazanych z lisciem
			for (ListOfExamples::const_iterator iter = queueHead.examples->begin();
					iter != queueHead.examples->end(); ++iter) {
				probability[iter->number][leafCategory] += (iter->weight * multiplier);
			}
			delete queueHead.examples; // usuniecie przykladow zwiazanych z lisciem
			continue;
		}
		// Utworzenie struktury na przyklady dla dzieci
		size_t childrenCount = queueHead.node->getChildrenCount();	// liczba dzieci
		vector<ListOfExamples*> childrenExamples = vector<ListOfExamples*>(
				childrenCount);
		for (size_t i = 0; i < childrenExamples.size(); ++i)
			childrenExamples[i] = new ListOfExamples();
		// Podzielenie przykladow
		for (ListOfExamples::iterator iter = queueHead.examples->begin();
				iter != queueHead.examples->end(); ++iter) {
			int value = table[iter->number][queueHead.node->getTest()];
			if (value != -1 &&
					queueHead.node->getChildAt(static_cast<size_t>(value)) != NULL) {
				childrenExamples[static_cast<size_t>(value)]->pushBack(Example(iter->number, iter->weight));
			}
			else {	// brakujaca wartosc lub brak wezla dla tej wartosci
				for (size_t i = 0; i < childrenCount; ++i) {
					Node* child = queueHead.node->getChildAt(i);
					if (child != NULL) {
						float multiplier = child->getExamplesCount() / queueHead.node->getExamplesCount();
						childrenExamples[i]->pushBack(Example(iter->number, iter->weight * multiplier));
					}
				}
			}
		}
		// Dodanie do kolejki wszystkich dzieci z ich przykladami
		for (size_t i = 0; i < childrenExamples.size(); ++i) {
			if (0 == childrenExamples[i]->size()) { // nie bylo przykladu o tej wartosci atrybutu
				delete childrenExamples[i];
				continue;
			}
			q.push(UnvisitedNode(queueHead.node->getChildAt(i), childrenExamples[i]));
		}
		delete queueHead.examples; // usuniecie przykladow zwiazanych z wezlem
	}

	// Wybranie najbardziej prawdopodobnej kategorii dla kazdego z przykladow
	vector<string> categories = vector<string>(examples.rows());
	for (size_t i = 0; i < categories.size(); ++i) {
		float max = 0.0f;
		size_t maxIndex = 0;
		for (size_t j = 0; j < categoryCount; ++j) {
			if (probability[i][j] > max) {
				max = probability[i][j];
				maxIndex = j;
			}
		}
#ifdef DEBUG
		BOOST_ASSERT(max != 0.0f);
#endif
		categories[i] = values_[categoryIndex_][maxIndex];
	}
	// Zwrocenie najbardziej prawdopodobnej kategorii dla kazdego z przykladow
	return categories;
}
