/*
 * DecisionTree.cpp
 */

#define DEBUG

#ifdef DEBUG
#include <iostream>
#include <boost/assert.hpp>

const float EPSILON = 0.00001;
#endif

#include "DecisionTree.hpp"
#include "UnvisitedNode.hpp"
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include <queue>
#include <cmath>	// log10()

using namespace id3lib;
using namespace std;
using namespace boost;

#ifdef DEBUG
void printTree(PNode node, size_t depth) {
	for (size_t i = 0; i < depth; ++i)
		cout << "*******";
	if (!node->isLeaf())
		cout << "t:" << node->getTest() << " c:" << node->getCategory() << endl;
	else
		cout << "LEAF c:" << node->getCategory() << endl;
	size_t new_depth = ++depth;
	for (size_t i = 0; i < node->getChildrenCount(); ++i) {
		PNode child(node->getChildAt(i));
		if (child != NULL)
			printTree(child, new_depth);
	}
}

void testTree(PNode node) {
	if (NULL == node)
		return;
	if (node->isLeaf())
		return;
	cout << "t:" << node->getTest() << " c:" << node->getCategory() << " ";
	float sum = 0.0f;
	for (size_t i = 0; i < node->getChildrenCount(); ++i) {
		PNode child(node->getChildAt(i));
		if (NULL != child)
			sum += child->getExamplesCount();
	}
	cout << "sumaOdDzieci=" << sum << " rodzic=" << node->getExamplesCount() << endl;
	BOOST_ASSERT(fabs(sum - node->getExamplesCount()) < EPSILON);
	for (size_t i = 0; i < node->getChildrenCount(); ++i)
		testTree(node->getChildAt(i));
}
#endif

DecisionTree::DecisionTree() :
		values_(vector<vector<string> >()), attributes_(vector<string>()), examplesCount_(0),
		attributesCount_(0), categoryIndex_(0), missingValueMark_(""), root() {

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
	shared_array<shared_array<int> > table(new shared_array<int> [examplesCount_]);
	for (size_t i = 0; i < examplesCount_; ++i)
		table[i] = shared_array<int>(new int[attributesCount_]);

	// Mapowanie stringow na int oraz tworzenie slownika
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
	PListOfExamples e(new ListOfExamples());
	for (size_t i = 0; i < examplesCount_; ++i)
		e->pushBack(Example(i, 1.0f));

#ifdef DEBUG
	BOOST_ASSERT(e->size() == examplesCount_);
	BOOST_ASSERT(e->weightSum() == static_cast<float>(examplesCount_));
	for (ListOfExamples::const_iterator iter = e->begin(); iter != e->end(); ++iter)
		BOOST_ASSERT(iter->weight == 1.0f);
#endif

	// Utworzenie listy dotychczas niewykorzystanych testow
	PListOfTests t(new ListOfTests());
	for (size_t i = 0; i < attributesCount_; ++i)
		if (i != categoryIndex_) // nie testujemy kategorii
			t->push_back(i);

#ifdef DEBUG
	BOOST_ASSERT(t->size() == attributesCount_ - 1);
#endif

	// Utworzenie nowego drzewa
	root.reset(new Node(e->weightSum()));

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
#ifdef DEBUG
		BOOST_ASSERT(fabs(queueHead.node->getExamplesCount() - queueHead.examples->weightSum()) < EPSILON);
#endif
		// Zliczenie sumy wag przykladow dla poszczegolnych kategorii
		vector<float> categories = vector<float>(values_[categoryIndex_].size(),
				0.0f);
		for (ListOfExamples::const_iterator iter = queueHead.examples->begin();
				iter != queueHead.examples->end(); ++iter) {
#ifdef DEBUG
			BOOST_ASSERT(table[iter->number][categoryIndex_] >= 0);
#endif
			size_t index = static_cast<size_t>(table[iter->number][categoryIndex_]);
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
		bool oneCategory = true;
		for (ListOfExamples::const_iterator iter = queueHead.examples->begin();
				iter != queueHead.examples->end(); ++iter) {
#ifdef DEBUG
			BOOST_ASSERT(table[iter->number][categoryIndex_] >= 0);
#endif
			if (static_cast<size_t>(table[iter->number][categoryIndex_])
					!= queueHead.node->getCategory()) {
				oneCategory = false;
				misclassifiedCounter += iter->weight;
			}
		}
		// Zapisanie liczby przykladow blednie klasyfikowanych
		queueHead.node->setMisclassifiedExamplesCount(misclassifiedCounter);
		// Sprawdzenie kryterium stopu
		if (oneCategory ||	// wszystkie przyklady jednej kategorii
				0 == queueHead.tests->size()) {	// lub koniec testow
			continue;
		}

		// Wybranie testu + podzial przykladow
		bool firstTime = true;
		size_t bestTest;
		float minEntropy;
		ListOfTests::iterator bestTestIterator;

		typedef vector<PListOfExamples> ExamplesForChildren;
		typedef shared_ptr<ExamplesForChildren> PExamplesForChildren;
		PExamplesForChildren saved;

		for (ListOfTests::iterator iter = queueHead.tests->begin();
				iter != queueHead.tests->end(); ++iter) {
			// Utworzenie struktury na przyklady dla dzieci
			size_t childrenCount = values_[*iter].size(); // liczba dzieci
			PExamplesForChildren current = PExamplesForChildren(new ExamplesForChildren(childrenCount));
			for (size_t i = 0; i < current->size(); ++i)
				(*current)[i].reset(new ListOfExamples);

			// glebokie kopiowanie przykladow zwiazanych z aktualnie przetwarzanym wezlem
			ListOfExamples currentExamples(*(queueHead.examples));

			// Podzielenie przykladow ze znanymi wartosciami wybranego testu
			for (ListOfExamples::iterator i = currentExamples.begin(); i != currentExamples.end();) {
				int value = table[i->number][*iter];
				if (value == -1) // nieznana wartosc
					++i;
				else {
					(*current)[static_cast<size_t>(value)]->pushBack(Example(i->number, i->weight));
					i = currentExamples.erase(i);
				}
			}

			// Wyznaczenie mnoznikow
			vector<float> multipliers = vector<float>(childrenCount, float(0.0f));
			float alreadySplitted = queueHead.node->getExamplesCount() - currentExamples.weightSum();
			for (size_t i = 0; i < multipliers.size(); ++i) {
				multipliers[i] = (*current)[i]->weightSum() / alreadySplitted;
			}

			// Podzielenie przykladow z nieznanymi wartosciami wybranego testu
			// jesli sa wylacznie przyklady z nieznanymi wartosciami to nie zostana podzielone
			for (ListOfExamples::const_iterator i = currentExamples.begin();
					i != currentExamples.end(); ++i) {
				for (size_t j = 0; j < current->size(); ++j) {
					if (0 == (*current)[j]->size()) // nie bylo przykladu o tej wartosci atrybutu
						continue;
					(*current)[j]->pushBack(Example(i->number, i->weight * multipliers[j]));
				}
			}

			// Obliczenie entropii
			// jesli zadne przyklady nie zostaly podzielone to entropia pozostanie 0.0f
			float entropy = 0.0f;
			for (size_t i = 0; i < current->size(); ++i) {
				if ((*current)[i]->size() == 0)	// brak przykladow o takim wyniku testu
					continue;
				// Entropia w obrebie i-tego wyniku testu
				float ent = 0.0f;
				// Zliczenie liczby przykladow poszczegolnych kategorii w obrebie
				// i-tego wyniku testu
				vector<float> categories = vector<float>(values_[categoryIndex_].size(), 0.0f);
				for (ListOfExamples::const_iterator j = (*current)[i]->begin();
					j != (*current)[i]->end(); ++j) {
					size_t cat = static_cast<size_t>(table[j->number][categoryIndex_]);
					categories[cat] += j->weight;
				}
				// Obliczenie entropii w obrebie i-tego wyniku testu
				for (size_t j = 0; j < categories.size(); ++j) {
					float divided = categories[j] / (*current)[i]->size();
					// TODO czy to jest niebezpieczne porownanie? raczej nie bo dla malych wartosci,
					// ale roznych od 0.0f logarytm nie zwraca -inf a poprostu duza liczbe ujemna
					if (divided != 0.0f)
						ent -= divided * log10(divided);
				}
				entropy += (*current)[i]->weightSum() / queueHead.node->getExamplesCount() * ent;
			}

			if (firstTime || entropy < minEntropy) {
				firstTime = false;
				bestTest = *iter;
				minEntropy = entropy;
				bestTestIterator = iter;
				saved = current;
#ifdef DEBUG
				BOOST_ASSERT(!saved.unique());
				BOOST_ASSERT(saved.use_count() == 2);
#endif
			}
		}
#ifdef DEBUG
		BOOST_ASSERT(saved.unique());
		BOOST_ASSERT(saved.use_count() == 1);
#endif
		// Zapisanie wybranego testu w wezle
		queueHead.node->setTest(bestTest);
		// Usuniecie wybranego testu z listy niewykorzystanych testow
		queueHead.tests->erase(bestTestIterator);
		// Utworzenie dzieci i dodanie ich do kolejki -
		// jesli zadne przyklady nie sa podzielone to nic nie zostanie
		// dodane do kolejki i zostanie utworzony lisc
		vector<PNode> children = vector<PNode>(saved->size());
		bool isLeaf = true;
		for (size_t i = 0; i < saved->size(); ++i) {
			if (0 == (*saved)[i]->size()) {	// nie bylo przykladu o tej wartosci atrybutu
				continue;
			}
			isLeaf = false;
			children[i].reset(new Node((*saved)[i]->weightSum()));
			PListOfTests childrenTests(new ListOfTests(*(queueHead.tests)));	// gleboka kopia
			q.push(UnvisitedNode(children[i], (*saved)[i], childrenTests));
		}
		// Ustawienie rodzicowi wskazan na dzieci
		if (!isLeaf)	// o ile nie jest lisciem
			queueHead.node->setChildren(children);
	}

#ifdef DEBUG
	cout << "Drzewo: " << endl;
	printTree(root, 0);
	testTree(root);
#endif
}

void DecisionTree::minimumErrorPrunning(unsigned m) throw (logic_error) {
	// Sprawdzenie poprawnosci stanu obiektu
	if (NULL == root.get())
		throw logic_error("Decision tree must be built to prune.");

	recursiveMEP(root, m);
#ifdef DEBUG
	cout << "Drzewo: " << endl;
	printTree(root, 0);
#endif
}

float DecisionTree::recursiveMEP(PNode node, unsigned m) {
	float k = static_cast<float>(values_[categoryIndex_].size());	// liczba kategorii
	float nodeErrorRate = (node->getMisclassifiedExamplesCount() + m * (k - 1) / k)
			/ (node->getExamplesCount() + m);	// MEP error rate

	if (node->isLeaf())
		return nodeErrorRate;

	float subtreeErrorRate = 0.0f;
	for (size_t i = 0; i < node->getChildrenCount(); ++i) {
		PNode child(node->getChildAt(i));
		if (child != NULL) {
			float multiplier = child->getExamplesCount() / node->getExamplesCount();
			subtreeErrorRate += (multiplier * recursiveMEP(child, m));
		}
	}

	if (nodeErrorRate < subtreeErrorRate) {
		node->makeLeaf();
		return nodeErrorRate;
	}

	return subtreeErrorRate;
}

void DecisionTree::reducedErrorPrunning(const Table& examples) throw (logic_error) {
	// Sprawdzenie poprawnosci stanu obiektu
	if (NULL == root.get())
		throw logic_error("Decision tree must be built to prune.");
}

vector<string> DecisionTree::classify(const Table& examples) const throw (logic_error,
		invalid_argument) {
	// Sprawdzenie poprawnosci stanu obiektu
	if (NULL == root.get())
		throw logic_error("Decision tree must be built to classify examples.");
	// Sprawdzenie poprawnosci parametru
	if (0 == examples.rows()) // brak przykladow
		throw invalid_argument("Table must have 1 or more rows (1 or more examples).");
	if (examples.columns() != attributesCount_)
		throw invalid_argument(
				"Examples to classify must have the same number of attributes as decision tree.");
	for (size_t i = 0; i < examples.columns(); ++i)
		if (!(attributes_[i] == examples.getAttrAt(i)))
			throw invalid_argument(
					"Examples to classify must have the same names of attributes as decision tree.");

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
	PListOfExamples e(new ListOfExamples());
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
#ifdef DEBUG
			BOOST_ASSERT(all != 0.0f);
#endif
			float incorrect = queueHead.node->getMisclassifiedExamplesCount();
			// Obliczenie prawdopodobienstwa ze kategoria przykladu zaliczonego do tego liscia
			// jest kategoria wyznaczona dla tego liscia
			float multiplier = (all - incorrect) / all;
#ifdef DEBUG
			BOOST_ASSERT(multiplier <= 1.0f);
			BOOST_ASSERT(multiplier > 0.0f);
#endif
			size_t leafCategory = queueHead.node->getCategory();	// kategoria liscia
			// Zwiekszenie prawdopodobienstwa dla wszystkich przykladow zwiazanych z lisciem
			for (ListOfExamples::const_iterator iter = queueHead.examples->begin();
					iter != queueHead.examples->end(); ++iter) {
				probability[iter->number][leafCategory] += (iter->weight * multiplier);
			}
			continue;
		}
		// Utworzenie struktury na przyklady dla dzieci
		size_t childrenCount = queueHead.node->getChildrenCount();	// liczba dzieci
		vector<PListOfExamples> childrenExamples = vector<PListOfExamples>(childrenCount);
		for (size_t i = 0; i < childrenExamples.size(); ++i)
			childrenExamples[i].reset(new ListOfExamples());
		// Podzielenie przykladow
		for (ListOfExamples::const_iterator iter = queueHead.examples->begin();
				iter != queueHead.examples->end(); ++iter) {
			int value = table[iter->number][queueHead.node->getTest()];
			if (value != -1 &&
					queueHead.node->getChildAt(static_cast<size_t>(value)) != NULL) {
				childrenExamples[static_cast<size_t>(value)]->pushBack(Example(iter->number, iter->weight));
			}
			else {	// brakujaca wartosc lub brak wezla dla tej wartosci
				for (size_t i = 0; i < childrenCount; ++i) {
					PNode child(queueHead.node->getChildAt(i));
					if (child != NULL) {
						float multiplier = child->getExamplesCount() / queueHead.node->getExamplesCount();
#ifdef DEBUG
						BOOST_ASSERT(multiplier <= 1.0f);
						BOOST_ASSERT(multiplier > 0.0f);
#endif
						childrenExamples[i]->pushBack(Example(iter->number, iter->weight * multiplier));
					}
				}
			}
		}
		// Dodanie do kolejki wszystkich dzieci z ich przykladami
		for (size_t i = 0; i < childrenExamples.size(); ++i) {
			if (0 == childrenExamples[i]->size()) { // nie bylo przykladu o tej wartosci atrybutu
				continue;
			}
			q.push(UnvisitedNode(queueHead.node->getChildAt(i), childrenExamples[i]));
		}
	}
#ifdef DEBUG
	cout << "probability:" << endl;
	for (size_t i = 0; i < probability.size(); ++i) {
		float probabilitySum = 0.0f;
		for (size_t j = 0; j < probability[i].size(); ++j) {
			probabilitySum += probability[i][j];
			cout << probability[i][j] << " ";
		}
		cout << endl;
		BOOST_ASSERT(probabilitySum <= 1.0f);
	}
#endif
	// Wybranie najbardziej prawdopodobnej kategorii dla kazdego z przykladow
	vector<string> bestCategories = vector<string>(examples.rows());
	for (size_t i = 0; i < bestCategories.size(); ++i) {
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
		bestCategories[i] = values_[categoryIndex_][maxIndex];
	}
	// Zwrocenie najbardziej prawdopodobnej kategorii dla kazdego z przykladow
	return bestCategories;
}
