/*
 * DecisionTree.cpp
 */

#define DEBUG

#ifdef DEBUG
#include <iostream>
#endif

#include "DecisionTree.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <list>
#include <utility>
#include <queue>

using namespace id3lib;
using namespace std;
using namespace boost;

DecisionTree::DecisionTree() :
		values_(vector<vector<string> >()), attributes_(vector<string>()), examplesCount_(
				0), attributesCount_(0), categoryIndex_(0), missingValueMark_(
				""), root(NULL) {

}

DecisionTree::~DecisionTree() {

}

void DecisionTree::build(const Table& examples, size_t categoryIndex,
		const std::string& missingValueMark) {
	// Inicjalizacja
	vector<vector<string> > values_ = vector<vector<string> >(
			examples.columns());
	vector<string> attributes_ = examples.getAttr();
	examplesCount_ = examples.rows();
	attributesCount_ = examples.columns();
	categoryIndex_ = categoryIndex;
	missingValueMark_ = missingValueMark;

	// Utworzenie tablicy na przyklady
	shared_array<shared_array<size_t> > table(
			new shared_array<size_t> [examplesCount_]);
	for (size_t i = 0; i < examplesCount_; ++i)
		table[i] = shared_array<size_t>(new size_t[attributesCount_]);

	// Mapowanie stringow na size_t
	// oraz zapamietywanie wystepujacych wartosci atrybutow
	for (size_t i = 0; i < attributesCount_; ++i) {
		for (size_t j = 0; j < examplesCount_; ++j) {
			size_t k = 0;
			for (; k < values_[i].size(); ++k) { // sprawdzenie czy taka wartosc juz wystapila
				if (examples.getValueAt(j, i) == values_[i][k])
					break;
			}
			if (k < values_[i].size()) // taka wartosc juz wystapila
				table[j][i] = k;
			else { // zapamietanie nowej wartosci
				values_[i].push_back(examples.getValueAt(j, i));
				table[j][i] = values_[i].size() - 1;
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

	// Utworzyc strukture wskazujaca na przyklady zwiazane z wezlem
	// (lista par: indeks przykladu, waga)
	list<pair<size_t, float> >* e = new list<pair<size_t, float> >();
	for (size_t i = 0; i < examples.rows(); ++i)
		(*e).push_back(pair<size_t, float>(i, 1.0f));
	delete root;
	// Utworzyc wezel root
	root = new Node(0, e->size());
	// Dodac wskazanie na wezel root i na strukture
	// przykladow z nim zwiazanych do kolejki FIFO
	queue<pair<Node*, list<pair<size_t, float> >* > > q = queue<pair<Node*, list<pair<size_t, float> >* > >();
	q.push(pair<Node*, list<pair<size_t, float> >* >(root, e));
	// Petla: Dopoki sa w kolejce wezly wraz z przykladami z nimi
	// zwiazanymi, przetwarzac kolejne wezly dodajac do
	// kolejki wskazania na ich dzieci wraz z przykladami z nimi
	// zwiazanymi
	// !Pod koniec przetwarzania wezla usuwac strukture wskazujaca
	// na przyklady zwiazane z tym wezlem
	while (!q.empty()) {
		pair<Node*, list<pair<size_t, float> >* > next = q.pop();
		// Jesli pusta lista przykladow to ja usunac i break
		// (wtedy misclassifiedExamplesCount pozostaje 0 (?))
		// (pozostaje kategoria odziedziczona po rodzicu)
		// Wybrac kategorie
		// Jesli wszystkie jednej kategorii to usunac liste przykladow i break
		// Zbadac ile przykladow jest blednie klasyfikowanych
		// Wybrac test
		// Podzielic przyklady ze znanymi wartosciami wyniku testu
		// (vector list)
		// Podzielic przyklady z nieznanymi wartosciami
		// Dodac do kolejki wszystkie dzieci z ich przykladami oraz
		// usunac strukture przykladow zwiazanych z wezlem
	}
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
