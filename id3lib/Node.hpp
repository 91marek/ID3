/*
 * Node.hpp
 */

#ifndef NODE_HPP_
#define NODE_HPP_

#include <vector>
#include <boost/shared_ptr.hpp>

namespace id3lib {

class Node;
typedef boost::shared_ptr<Node> PNode;

class Node {
public:
	/*
	 * Tworzy wezel o danej liczbie przykladow z nim zwiazanych
	 */
	Node(float examplesCount) :
			children_(std::vector<PNode>()), category_(0), examplesCount_(
					examplesCount), misclassifiedExamplesCount_(0.0f), test_(0) {

	}

	size_t getChildrenCount() const {
		return children_.size();
	}

	PNode getChildAt(size_t i) const {
		return children_[i];
	}

	void setChildren(const std::vector<PNode>& children) {
		children_ = children;
	}

	size_t getCategory() const {
		return category_;
	}

	void setCategory(size_t category) {
		category_ = category;
	}

	float getExamplesCount() const {
		return examplesCount_;
	}

	float getMisclassifiedExamplesCount() const {
		return misclassifiedExamplesCount_;
	}

	void setMisclassifiedExamplesCount(float misclassifiedExamplesCount) {
		misclassifiedExamplesCount_ = misclassifiedExamplesCount;
	}

	size_t getTest() const {
		return test_;
	}

	void setTest(size_t test) {
		test_ = test;
	}

	bool isLeaf() const {
		return 0 == children_.size();
	}

	void makeLeaf() {
		children_ = std::vector<PNode>();
	}
protected:
	/*
	 * Wskazania na dzieci
	 */
	std::vector<PNode> children_;
	/*
	 * Kategoria wezla
	 */
	size_t category_;
	/*
	 * Liczba przykladow zbioru trenujacego
	 * zwiazanych z wezlem
	 */
	const float examplesCount_;
	/*
	 * Liczba przykladow zbioru trenujacego
	 * blednie klasyfikowanych przez
	 * kategorie wezla
	 */
	float misclassifiedExamplesCount_;
	/*
	 * Testowany atrybut
	 */
	size_t test_;
};

} /* namespace id3lib */
#endif /* NODE_HPP_ */
