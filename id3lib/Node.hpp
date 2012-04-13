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
					examplesCount), misclassifiedExamplesCount_(0) {

	}
	/*
	 * Wywoluje destruktor wszystkich dzieci
	 */
	~Node() {
		size_t size = children_.size();
		for (size_t i = 0; i < size; ++i)
			delete children_[i];
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

	size_t getExamplesCount() const {
		return examplesCount_;
	}

	size_t getMisclassifiedExamplesCount() const {
		return misclassifiedExamplesCount_;
	}

	void setMisclassifiedExamplesCount(size_t misclassifiedExamplesCount) {
		misclassifiedExamplesCount_ = misclassifiedExamplesCount;
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
	const size_t examplesCount_;
	/*
	 * Liczba przykladow zbioru trenujacego
	 * blednie klasyfikowanych przez
	 * kategorie wezla
	 */
	size_t misclassifiedExamplesCount_;
};

} /* namespace id3lib */
#endif /* NODE_HPP_ */
