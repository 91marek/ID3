/*
 * DecisionTree.cpp
 */

#ifdef DEBUG
#include <boost/assert.hpp>
const float EPSILON = 0.00001;
#endif

#include "DecisionTree.hpp"
#include "UnvisitedNode.hpp"
#include <iostream>
#include <queue>
#include <cmath>	// log10()

using namespace id3lib;
using std::vector;
using std::string;
using std::invalid_argument;
using boost::shared_array;
using std::queue;
using std::numeric_limits;
using std::istream;
using boost::shared_ptr;
using std::logic_error;
using std::ostream;
using std::endl;

#ifdef DEBUG
void testTree(PNode node) {
	if (NULL == node.get())
		return;
	if (node->isLeaf())
		return;
	float sum = 0.0f;
	for (size_t i = 0; i < node->getChildrenCount(); ++i) {
		PNode child(node->getChildAt(i));
		if (NULL != child.get())
			sum += child->getExamplesCount();
	}
	BOOST_ASSERT(fabs(sum - node->getExamplesCount()) < EPSILON);
	for (size_t i = 0; i < node->getChildrenCount(); ++i)
		testTree(node->getChildAt(i));
}
#endif

DecisionTree::DecisionTree() :
		values_(vector<vector<string> >()), attributes_(vector<string>()), examplesCount_(0),
		attributesCount_(0), categoryIndex_(0), missingValueMark_(""), root() {

}

void DecisionTree::build(const TrainingSet& examples) throw (invalid_argument) {
	// Sprawdzenie poprawnosci parametru
	if (0 == examples.rows()) // brak przykladow w zbiorze trenujacym
		throw invalid_argument("Training set must have 1 or more rows (1 or more examples).");

	// Inicjalizacja
	values_ = vector<vector<string> >(examples.columns());
	attributes_ = examples.getAttr();
	examplesCount_ = examples.rows();
	attributesCount_ = examples.columns();
	categoryIndex_ = examples.getCategoryIndex();
	missingValueMark_ = examples.getMissingValueMark();

	// Utworzenie tablicy na przyklady
	shared_array<shared_array<int> > table = makeTable(examplesCount_, attributesCount_);

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
	cout << "table:" << endl;
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
		BOOST_ASSERT(queueHead.node->getExamplesCount() == queueHead.examples->weightSum());
#endif
		// Zliczenie sumy wag przykladow dla poszczegolnych kategorii
		vector<float> categories = vector<float>(values_[categoryIndex_].size(), 0.0f);
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
		size_t bestTest = 0;
		float minEntropy = numeric_limits<float>::max();
		ListOfTests::iterator bestTestIterator;

		PExamplesForChildren saved;

		for (ListOfTests::iterator iter = queueHead.tests->begin();
				iter != queueHead.tests->end(); ++iter) {
			// Podzielenie przykladow wg danego testu
			PExamplesForChildren current(split(*(queueHead.examples), *iter, table));

			// Obliczenie entropii
			// jesli zadne przyklady nie zostaly podzielone to entropia maksymalna
			float entropy = 0.0f;
			bool notSplitted = true;
			for (size_t i = 0; i < current->size(); ++i) {
				if ((*current)[i]->size() == 0)	{	// brak przykladow o takim wyniku testu
					continue;
				}
				notSplitted = false;	// przynajmniej jeden przyklad bez brakujacej wartosci dla danego testu
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
					if (categories[j] == 0.0f) {
						continue;
					}
					float divided = categories[j] / (*current)[i]->weightSum();
					ent -= divided * log10(divided);
				}
#ifdef DEBUG
				cout << "ent: " << ent << " ";
#endif
				entropy += (*current)[i]->weightSum() / queueHead.node->getExamplesCount() * ent;
			}
			// maksymalna entropia jesli podzial nie dzieli przykladow
			// (ze wzgledu na same brakujace wartosci)
			if (notSplitted) {
				entropy = numeric_limits<float>::max();
			}
#ifdef DEBUG
			cout << endl << "entropy: " << entropy << endl;
#endif
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
		cout << "minEntropy: " << minEntropy << endl << endl;
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
	cout << "Drzewo:" << endl << *this;
	testTree(root);
#endif
}

void DecisionTree::build(istream& is, const string& separator) throw (invalid_argument) {
	// TODO
	return;
}

shared_ptr<vector<string> > DecisionTree::classify(const Table& examples) const throw (logic_error,
		invalid_argument) {
	// Sprawdzenie poprawnosci stanu obiektu
	if (NULL == root.get())
		throw logic_error("Decision tree must be built to classify examples.");
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
	vector<string>* bestCategories = new vector<string>(examples.rows());
	for (size_t i = 0; i < bestCategories->size(); ++i) {
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
		(*bestCategories)[i] = values_[categoryIndex_][maxIndex];
	}
	// Zwrocenie najbardziej prawdopodobnej kategorii dla kazdego z przykladow
	return shared_ptr<vector<string> >(bestCategories);
}

void DecisionTree::recursivePrintTree(ostream& os, PNode node, size_t depth, size_t test, size_t result) const {
	for (size_t i = 0; i < depth; ++i)
		os << "#";
	os << "(";
	if (0 == depth)
		os << "|";
	else
		os << values_[test][result] << "|";
	if (!node->isLeaf())
		os << values_[categoryIndex_][node->getCategory()] << "|" << attributes_[node->getTest()];
	else
		os << values_[categoryIndex_][node->getCategory()] << "|";
	os << ")" << endl;
	size_t new_depth = ++depth;
	for (size_t i = 0; i < node->getChildrenCount(); ++i) {
		PNode child(node->getChildAt(i));
		if (child != NULL)
			recursivePrintTree(os, child, new_depth, node->getTest(), i);
	}
}

shared_array<shared_array<int> > DecisionTree::makeTable(size_t rows, size_t columns) const {
	shared_array<shared_array<int> > table(new shared_array<int> [rows]);
	for (size_t i = 0; i < rows; ++i)
		table[i] = shared_array<int>(new int[columns]);
	return table;
}

shared_array<shared_array<int> > DecisionTree::map(const Table& examples) const {
	// Utworzenie tablicy na przyklady
	shared_array<shared_array<int> > table = makeTable(examples.rows(), examples.columns());

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
	return table;
}

std::ostream& id3lib::operator<<(std::ostream& os, const DecisionTree& dt) {
	if (NULL == dt.root.get())
		os << "Decision tree is not built." << endl;
	else
		dt.recursivePrintTree(os, dt.root, 0, 0, 0);
	return os;
}
