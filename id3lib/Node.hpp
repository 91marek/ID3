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
	 * Tworzy wezel o danej kategorii i danej
	 * liczbie przykladow z nim zwiazanych
	 */
	Node(size_t category, size_t examplesCount) :
			children_(std::vector<Node*>()), category_(category), examplesCount_(
					examplesCount), misclassifiedExamplesCount_(0.0f), test_(0) {

	}
	/*
	 * Wywoluje destruktor wszystkich dzieci
	 */
	~Node() {
		size_t size = children_.size();
		for (size_t i = 0; i < size; ++i)
			delete children_[i];
	}

	size_t getChildCount() const {
		return children_.size();
	}

	Node* getChildAt(size_t i) const {
		return children_[i];
	}

	void setChildAt(Node* child, size_t i) {
		children_[i] = child;
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
protected:
	/*
	 * Wskazania na dzieci
	 */
	std::vector<Node*> children_;
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
