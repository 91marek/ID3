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

int main() {
	cout << "Hello ID3!" << endl;
	DecisionTree dt;
	dt.build();
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
