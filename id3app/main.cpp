/*
 * main.cpp
 */

#define DEBUG

#include <iostream>
#include <id3lib/DecisionTree.hpp>
#include <id3lib/Table.hpp>

using namespace std;
using namespace id3lib;

int main() {
	cout << "Hello ID3!" << endl;
	vector<string> attributes = vector<string>();
	attributes.push_back("outlook");
	attributes.push_back("temperature");
	attributes.push_back("humidity");
	attributes.push_back("wind");
	attributes.push_back("play");
	Table examples = Table(attributes);
	vector<string> ex0 = vector<string>();
	ex0.push_back("sunny");
	ex0.push_back("?");
//	ex0.push_back("hot");
	ex0.push_back("high");
	ex0.push_back("normal");
	ex0.push_back("no");
	vector<string> ex1 = vector<string>();
//	ex1.push_back("sunny");
	ex1.push_back("?");
	ex1.push_back("hot");
	ex1.push_back("high");
	ex1.push_back("high");
	ex1.push_back("no");
	vector<string> ex2 = vector<string>();
//	ex2.push_back("overcast");
	ex2.push_back("?");
	ex2.push_back("hot");
	ex2.push_back("high");
	ex2.push_back("normal");
	ex2.push_back("yes");
	vector<string> ex3 = vector<string>();
	ex3.push_back("rainy");
	ex3.push_back("?");
	ex3.push_back("mild");
//	ex3.push_back("high");
	ex3.push_back("normal");
	ex3.push_back("yes");
	vector<string> ex4 = vector<string>();
	ex4.push_back("rainy");
//	ex4.push_back("?");
	ex4.push_back("cold");
	ex4.push_back("normal");
	ex4.push_back("normal");
	ex4.push_back("yes");
	vector<string> ex5 = vector<string>();
	ex5.push_back("rainy");
//	ex5.push_back("?");
	ex5.push_back("cold");
	ex5.push_back("normal");
	ex5.push_back("high");
	ex5.push_back("no");
	examples.pushBack(ex0);
	examples.pushBack(ex1);
	examples.pushBack(ex2);
	examples.pushBack(ex3);
	examples.pushBack(ex4);
	examples.pushBack(ex5);
#ifdef DEBUG
	cout << "examples:" << endl;
	for (size_t i = 0; i < examples.rows(); ++i) {
		for (size_t j = 0; j < examples.columns(); ++j) {
			cout << examples.getValueAt(i, j) << " ";
		}
		cout << endl;
	}
#endif
	DecisionTree dt = DecisionTree();
	dt.build(examples, 4, "?");
	vector<string> categories = dt.classify(examples);
	cout << "Wynik klasyfikacji:" << endl;
	for (size_t i = 0; i < categories.size(); ++i) {
		string before = examples.getValueAt(i, 4);
		string after = categories[i];
		cout << "pierwotnie: " << before << " wynik: " << after;
		if (!(before == after))
			cout << " INACZEJ";
		else
			cout << " TAK SAMO";
		cout << endl;
	}
	cout << "Przycinanie" << endl;
	dt.minimumErrorPrunning(dt.getCategoryCount());
	cout << "Good Bye ID3!" << endl;
	return 0;
}
