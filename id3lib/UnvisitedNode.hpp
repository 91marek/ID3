/*
 * UnvisitedNode.hpp
 */

#ifndef UNVISITEDNODE_HPP_
#define UNVISITEDNODE_HPP_

#include "Node.hpp"
#include "ListOfExamples.hpp"
#include <list>
#include <boost/shared_ptr.hpp>

namespace id3lib {

typedef boost::shared_ptr<ListOfExamples> PListOfExamples;

typedef std::list<size_t> ListOfTests;
typedef boost::shared_ptr<ListOfTests> PListOfTests;

/*
 * Klasa reprezentujaca nieodwiedzone wezly zarowno
 * podczas budowy jak i klasyfikacji
 */
class UnvisitedNode {
public:
	UnvisitedNode(PNode n, PListOfExamples e, PListOfTests t = PListOfTests()) :
			node(n), examples(e), tests(t) {

	}
	/*
	 * Wezel
	 */
	const PNode node;
	/*
	 * Przyklady zwiazane z wezlem
	 */
	const PListOfExamples examples;
	/*
	 * Niewykorzystane testy
	 */
	const PListOfTests tests;
};

} /* namespace id3lib */
#endif /* UNVISITEDNODE_HPP_ */
