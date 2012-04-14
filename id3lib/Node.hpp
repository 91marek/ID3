/*
 * Node.hpp
 */

#ifndef NODE_HPP_
#define NODE_HPP_

#include <vector>

namespace id3lib {

class Node {
public:
	/*
	 * Tworzy wezel o danej liczbie przykladow z nim zwiazanych
	 */
	Node(float examplesCount) :
			children_(std::vector<Node*>()), category_(-1), examplesCount_(
					examplesCount), misclassifiedExamplesCount_(0.0f), test_(0) {

	}
	/*
	 * Wywoluje destruktor wszystkich dzieci
	 */
	~Node() {
		for (size_t i = 0; i < children_.size(); ++i)
			delete children_[i];
	}

	size_t getChildrenCount() const {
		return children_.size();
	}

	const std::vector<Node*>& getChildren() const {
		return children_;
	}

	void setChildren(const std::vector<Node*>& children) {
		children_ = children;
	}

	Node* getChildAt(size_t i) const {
		return children_[i];
	}

	void setChildAt(Node* child, size_t i) {
		children_[i] = child;
	}

	int getCategory() const {
		return category_;
	}

	void setCategory(int category) {
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
protected:
	/*
	 * Wskazania na dzieci
	 */
	std::vector<Node*> children_;
	/*
	 * Kategoria wezla
	 */
	int category_;
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
