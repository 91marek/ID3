/*
 * main.cpp
 */

#include <iostream>

#include <id3lib/DecisionTree.hpp>

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
	return 0;
}
