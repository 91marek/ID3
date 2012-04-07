/*
 * main.cpp
 */

#include <iostream>

#include <id3lib/DecisionTree.hpp>

int main() {
	std::cout << "Hello ID3!" << std::endl;
	DecisionTree dt;
	dt.build();
	return 0;
}
