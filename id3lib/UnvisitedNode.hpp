/*
 * UnvisitedNode.hpp
 */

#ifndef UNVISITEDNODE_HPP_
#define UNVISITEDNODE_HPP_

#include "Node.hpp"
#include "ListOfExamples.hpp"
#include <list>

namespace id3lib {

typedef std::list<size_t> ListOfTests;

class UnvisitedNode {
public:
	UnvisitedNode(Node* n, ListOfExamples* e, ListOfTests* t) :
			node(n), examples(e), tests(t) {

	}
	/*
	 * Wezel
	 */
	Node* const node;
	/*
	 * Przyklady zwiazane z wezlem
	 */
	ListOfExamples* const examples;
	/*
	 * Niewykorzystane testy
	 */
	ListOfTests* const tests;
};

} /* namespace id3lib */
#endif /* UNVISITEDNODE_HPP_ */
