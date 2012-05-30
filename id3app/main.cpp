/*
 * main.cpp
 */

#include <iostream>
#include <fstream>
#include <id3lib/DecisionTree.hpp>
#include <id3lib/ZPRDecisionTree.hpp>
#include <id3lib/UMDecisionTree.hpp>
#include <stdexcept>

using std::vector;
using std::string;
using std::invalid_argument;
using boost::shared_array;
using std::numeric_limits;
using std::istream;
using boost::shared_ptr;
using std::logic_error;
using std::ostream;
using std::endl;
using std::cout;
using std::cerr;
using std::ifstream;
using namespace id3lib;

int main() {
	cout << "Hello ID3!" << endl;

	/* ---ENJOY_SPORT--- */
	vector<string> attributes = vector<string>();
	attributes.push_back("outlook");
	attributes.push_back("temperature");
	attributes.push_back("humidity");
	attributes.push_back("wind");
	attributes.push_back("enjoy_sport");
	TrainingSet examples = TrainingSet(attributes, 4, "?");
	vector<string> ex0 = vector<string>();
	ex0.push_back("sunny");
	ex0.push_back("?");
//	ex0.push_back("hot");
	ex0.push_back("high");
	ex0.push_back("normal");
	ex0.push_back("no");
	vector<string> ex1 = vector<string>();
	ex1.push_back("sunny");
	ex1.push_back("?");
//	ex1.push_back("hot");
	ex1.push_back("high");
	ex1.push_back("high");
	ex1.push_back("no");
	vector<string> ex2 = vector<string>();
	ex2.push_back("overcast");
	ex2.push_back("?");
//	ex2.push_back("hot");
	ex2.push_back("high");
	ex2.push_back("normal");
	ex2.push_back("yes");
	vector<string> ex3 = vector<string>();
	ex3.push_back("rainy");
	ex3.push_back("?");
//	ex3.push_back("mild");
	ex3.push_back("high");
	ex3.push_back("normal");
	ex3.push_back("yes");
	vector<string> ex4 = vector<string>();
	ex4.push_back("rainy");
	ex4.push_back("?");
//	ex4.push_back("cold");
	ex4.push_back("normal");
	ex4.push_back("normal");
	ex4.push_back("yes");
	vector<string> ex5 = vector<string>();
	ex5.push_back("rainy");
	ex5.push_back("?");
//	ex5.push_back("cold");
	ex5.push_back("normal");
	ex5.push_back("high");
	ex5.push_back("no");
	examples.pushBack(ex0);
	examples.pushBack(ex1);
	examples.pushBack(ex2);
	examples.pushBack(ex3);
	examples.pushBack(ex4);
	examples.pushBack(ex5);
	cout << "examples:" << endl;
	for (size_t i = 0; i < examples.rows(); ++i) {
		for (size_t j = 0; j < examples.columns(); ++j) {
			cout << examples.getValueAt(i, j) << " ";
		}
		cout << endl;
	}
	UMDecisionTree dt = UMDecisionTree();
	dt.build(examples);
	dt.minimumErrorPruning(dt.getCategoryCount());
	shared_ptr<vector<string> > categories(dt.classify(examples));
	ErrorRate err = ErrorRate();
	cout << "Wynik klasyfikacji:" << endl;
	for (size_t i = 0; i < categories->size(); ++i) {
		string before = examples.getValueAt(i, 4);
		string after = (*categories)[i];
		cout << "pierwotnie: " << before << " wynik: " << after;
		if (!(before == after)) {
			cout << " INACZEJ";
			err.misclassifiedExample();
		}
		else {
			cout << " TAK SAMO";
			err.correctlyClassifiedExample();
		}
		cout << endl;
	}
	cout << "Error rate: " << err.get() << endl;
	dt.reducedErrorPruning(examples);

	/* ---ENJOY_SPORT--- */
	TrainingSet e = TrainingSet(attributes, 4, "?");
	try {
		ifstream file("enjoy_sport.txt");
		e.readFromStream(file, ", ");
		ZPRDecisionTree dt3 = ZPRDecisionTree();
		dt3.build(e);
	} catch (const std::exception& e) {
		cerr << e.what() << endl;
	}

	/* ---EAT_MUSHROOM--- */
	vector<string> attr = vector<string>();
	attr.push_back("eat");
	attr.push_back("cap-shape");
	attr.push_back("cap-surface");
	attr.push_back("cap-color");
	attr.push_back("bruises");
	attr.push_back("odor");
	attr.push_back("gill-attachment");
	attr.push_back("gill-spacing");
	attr.push_back("gill-size");
	attr.push_back("gill-color");
	attr.push_back("stalk-shape");
	attr.push_back("stalk-root");
	attr.push_back("stalk-surface-above-ring");
	attr.push_back("stalk-surface-below-ring");
	attr.push_back("stalk-color-above-ring");
	attr.push_back("stalk-color-below-ring");
	attr.push_back("veil-type");
	attr.push_back("veil-color");
	attr.push_back("ring-number");
	attr.push_back("ring-type");
	attr.push_back("spore-print-color");
	attr.push_back("population");
	attr.push_back("habitat");
	TrainingSet mushroom_table = TrainingSet(attr, 0, "?");
	try {
		ifstream file2("eat_mushroom.txt");
		size_t examplesCount = mushroom_table.readFromStream(file2, ", ");
		ZPRDecisionTree mushroom_dt = ZPRDecisionTree();
		mushroom_dt.build(mushroom_table);
		shared_ptr<vector<string> > result(mushroom_dt.classify(mushroom_table));
		ErrorRate er = ErrorRate();
		er.count(result, mushroom_table);
		cout << "Liczba wczytanych przykladow: " << examplesCount << " Error rate: " << er.get() << endl;
	} catch (std::exception& e) {
		cerr << e.what() << endl;
	}

	cout << "Good Bye ID3!" << endl;
	return 0;
}
