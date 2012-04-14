/*
 * DecisionTree.cpp
 */

#define DEBUG

#ifdef DEBUG
#include <iostream>
#endif

#include "DecisionTree.hpp"
#include "Example.hpp"
#include <boost/shared_array.hpp>
#include <boost/assert.hpp>
#include <list>
#include <utility>	// std::pair
#include <queue>

using namespace id3lib;
using namespace std;
using namespace boost;

#ifdef DEBUG
void printTree(Node* node, size_t depth) {
	for (size_t i = 0; i < depth; ++i)
		cout << "***";
	cout << "test: " << node->getTest() << " cat: " << node->getCategory()
			<< endl;
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
		throw invalid_argument("Index of category column is out of bounds.");
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
	for (int y = 0; y < static_cast<int>(attributesCount_); ++y) {
		for (int x = 0; x < static_cast<int>(examplesCount_); ++x) {
			if (examples.getValueAt(x, y) == missingValueMark_) {
				table[x][y] = -1; // -1 dla brakujacych wartosci
				continue;
			}
			int k = 0;
			for (; k < static_cast<int>(values_[y].size()); ++k) { // sprawdzenie czy taka wartosc juz wystapila
				if (examples.getValueAt(x, y) == values_[y][k])
					break;
			}
			if (k < static_cast<int>(values_[y].size())) // taka wartosc juz wystapila
				table[x][y] = k;
			else { // zapamietanie nowej wartosci
				values_[y].push_back(examples.getValueAt(x, y));
				table[x][y] = values_[y].size() - 1;
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

	// Utworzenie struktury przechowujacej informacje
	// o przykladach zwiazanych z wezlem root
	typedef list<Example> ListOfExamples; // lista par: numer przykladu, waga
	ListOfExamples* e = new ListOfExamples();
	for (size_t i = 0; i < examplesCount_; ++i)
		e->push_back(Example(i, 1.0f));

#ifdef DEBUG
	BOOST_ASSERT(e->size() == examplesCount_);
	for (ListOfExamples::const_iterator iter = e->begin(); iter != e->end();
			++iter)
		BOOST_ASSERT(iter->weight == 1.0f);
#endif

	// Usuniecie biezacego drzewa
	delete root;
	// Utworzenie wezla root
	root = new Node(e->size()); // size() poniewaz wszystkie przyklady maja wage 1.0f
	// Dodanie do kolejki FIFO wskazania na wezel root
	// i na strukture przykladow z nim zwiazanych
	typedef pair<Node*, ListOfExamples*> NodeAndExamples;
	queue<NodeAndExamples> q = queue<NodeAndExamples>();
	q.push(NodeAndExamples(root, e));

	// Przetwarzanie drzewa wszerz
	size_t tempCounter = 0; // TODO wywalic po zaimplementowaniu wyboru testu
	while (!q.empty()) {
		NodeAndExamples queueHead = q.front();
		q.pop();
		// Zliczenie sumy wag przykladow dla poszczegolnych kategorii
		// oraz sumy wag wszystkich przykladow
		vector<float> categories = vector<float>(values_[categoryIndex_].size(),
				float(0.0f));

#ifdef DEBUG
		for (size_t i = 0; i < categories.size(); ++i) {
			BOOST_ASSERT(categories[i] == 0.0f);
		}
#endif

		float all = 0.0f; // suma wag wszystkich przykladow
		for (ListOfExamples::const_iterator iter = queueHead.second->begin();
				iter != queueHead.second->end(); ++iter) {
			categories[table[iter->number][categoryIndex_]] += iter->weight;
			all += iter->weight;
		}
		// Wybranie kategorii wiekszosciowej
		float max = 0.0f;
		int maxIndex = -1;
		for (size_t i = 0; i < categories.size(); ++i) {
			if (categories[i] > max) {
				max = categories[i];
				maxIndex = static_cast<int>(i);
			}
		}
		BOOST_ASSERT(max != 0.0f);
		BOOST_ASSERT(maxIndex != -1);
		// Zapisanie kategorii wiekszosciowej w wezle
		queueHead.first->setCategory(static_cast<size_t>(maxIndex));
		if (all == max) { // wszystkie przyklady jednej kategorii
			queueHead.first->setMisclassifiedExamplesCount(0.0f); // wszystkie poprawnie klasyfikowane
			delete queueHead.second; // usuniecie przykladow zwiazanych z wezlem
			continue;
		}
		// Obliczenie i zapisanie liczby przykladow blednie klasyfikowanych
		float misclassifiedCounter = 0.0f;
		for (ListOfExamples::const_iterator iter = queueHead.second->begin();
				iter != queueHead.second->end(); ++iter) {
			if (table[iter->number][categoryIndex_]
					!= static_cast<int>(queueHead.first->getCategory()))
				misclassifiedCounter += iter->weight;
		}
		queueHead.first->setMisclassifiedExamplesCount(misclassifiedCounter);
		// Wybranie testu
		size_t bestTest = 0;
		//double maxInformationGain = 0.0;
		// tymczasowo:
		if (tempCounter == categoryIndex_)
			++tempCounter;
		bestTest = tempCounter;
		++tempCounter;
		for (size_t i = 0; i < attributesCount_; ++i) {
			if (i == categoryIndex_) // nie testujemy kategorii
				continue;
			// TODO
			// bez sensu liczyc informationGain dla wszystkich atrybutow
			// bo w poddrzewie nie chcemy drugi raz uzywac tego samego testu
			// trzeba jakos przekazac ktore testy zostaly wykonane lub jakie pozostaly
		}

#ifdef DEBUG
		cout << "Wybrany test: " << attributes_[bestTest] << endl;
#endif

		queueHead.first->setTest(bestTest);
		// Struktura na przyklady dla dzieci
		size_t childrenCount = values_[bestTest].size(); // liczba dzieci
		vector<ListOfExamples> childrenExamples = vector<ListOfExamples>(
				childrenCount);

#ifdef DEBUG
		for (size_t i = 0; i < childrenExamples.size(); ++i) {
			BOOST_ASSERT(childrenExamples[i].size() == 0);
		}
#endif

		vector<float> weightSum = vector<float>(childrenCount, float(0.0f));
		// Podzielenie przykladow ze znanymi wartosciami wybranego testu
		for (ListOfExamples::iterator iter = queueHead.second->begin();
				iter != queueHead.second->end();) {
			int value = table[iter->number][bestTest];
			if (value == -1) // nieznana wartosc
				++iter;
			else {
				childrenExamples[value].push_back(
						Example(iter->number, iter->weight));
				weightSum[value] += iter->weight;
				iter = queueHead.second->erase(iter);
			}
		}
		// Podzielenie przykladow z nieznanymi wartosciami wybranego testu
		for (ListOfExamples::const_iterator iter = queueHead.second->begin();
				iter != queueHead.second->end(); ++iter) {
			for (size_t i = 0; i < childrenExamples.size(); ++i) {
				childrenExamples[i].push_back(
						Example(iter->number,
								iter->weight * weightSum[i] / all));
			}
		}
		// Dodanie do kolejki wszystkich dzieci z ich przykladami
		vector<Node*> children = vector<Node*>(childrenCount, NULL);
		for (size_t i = 0; i < childrenExamples.size(); ++i) {
			if (0 == childrenExamples[i].size()) // nie bylo przykladu o tej wartosci atrybutu
				continue;
			children[i] = new Node(weightSum[i]);
			ListOfExamples* e = new ListOfExamples(childrenExamples[i]);
			q.push(NodeAndExamples(children[i], e));
		}
		// Ustawienie rodzicowi wskazan na dzieci
		queueHead.first->setChildren(children);
		// Usuniecie struktury przechowujacej informacje
		// o przykladach zwiazanych z wezlem
		delete queueHead.second;
	}

#ifdef DEBUG
	cout << "Drzewo: " << endl;
	printTree(root, 0);
#endif
}

void DecisionTree::prune() {
	// TODO
	return;
}

void DecisionTree::prune(const Table& examples) {
	// TODO
	return;
}

vector<string> DecisionTree::classify(const Table& examples) const {
	// TODO
	return vector<string>();
}
