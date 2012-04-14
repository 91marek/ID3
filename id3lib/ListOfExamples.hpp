/*
 * ListOfExamples.hpp
 */

#ifndef LISTOFEXAMPLES_HPP_
#define LISTOFEXAMPLES_HPP_

#include "Example.hpp"
#include <list>

namespace id3lib {

class ListOfExamples {
public:
	typedef std::list<Example>::const_iterator const_iterator;
	typedef std::list<Example>::iterator iterator;

	ListOfExamples() : list_(std::list<Example>()), weightSum_(0.0f) {

	}

	void pushBack(const Example& example) {
		weightSum_ += example.weight;
		list_.push_back(example);
	}
	iterator erase(iterator position) {
		weightSum_ -= position->weight;
		return list_.erase(position);
	}
	const_iterator begin() const {
		return list_.begin();
	}
	iterator begin() {
		return list_.begin();
	}
	const_iterator end() const {
		return list_.end();
	}
	iterator end() {
		return list_.end();
	}
	size_t size() const {
		return list_.size();
	}
	float weightSum() const {
		return weightSum_;
	}
protected:
	std::list<Example> list_;
	float weightSum_;
};

} /* namespace id3lib */
#endif /* LISTOFEXAMPLES_HPP_ */
