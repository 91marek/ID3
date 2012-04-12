/*
 * main.cpp
 */

#include <iostream>

#include <id3lib/DecisionTree.hpp>
#include <id3lib/Table.hpp>

using namespace std;

string toString() {
	return "ex";
}

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
	vector<vector<string> > examples = vector<vector<string> >();
	vector<string> ex0 = vector<string>();
	ex0.push_back("sunny");
	ex0.push_back("hot");
	ex0.push_back("high");
	ex0.push_back("normal");
	ex0.push_back("no");
	vector<string> ex1 = vector<string>();
	ex1.push_back("sunny");
	ex1.push_back("hot");
	ex1.push_back("high");
	ex1.push_back("high");
	ex1.push_back("no");
	vector<string> ex2 = vector<string>();
	ex2.push_back("overcast");
	ex2.push_back("hot");
	ex2.push_back("high");
	ex2.push_back("normal");
	ex2.push_back("yes");
	examples.push_back(ex0);
	examples.push_back(ex1);
	examples.push_back(ex2);
	DecisionTree dt = DecisionTree(examples, attributes, attributes.size() - 1);
	cout << "Good Bye ID3!" << endl;
	cout << "wypis" << endl;
	vector<string> v = vector<string>();
	v.push_back("a");
	v.push_back("b");
	Table t = Table(v);
	v.push_back("c");
	for (unsigned int i = 0; i < v.size(); ++i) {
		cout << v[i] << endl;
	}
	vector<string> a = t.getAttr();
	for (unsigned int i = 0; i < a.size(); ++i) {
		cout << a[i] << endl;
	}
	a.push_back("d");
	vector<string> a2 = t.getAttr();
	for (unsigned int i = 0; i < a2.size(); ++i) {
		cout << a2[i] << endl;
	}
	cout << "ostatnia petla" << endl;
	for (unsigned int i = 0; i < a.size(); ++i) {
		cout << a[i] << endl;
	}
	return 0;
}
