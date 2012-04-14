/*
 * Example.hpp
 */

#ifndef EXAMPLE_HPP_
#define EXAMPLE_HPP_

namespace id3lib {

class Example {
public:
	Example(size_t n, float w) : number(n), weight(w) {

	}
	size_t number;
	float weight;
};

} /* namespace id3lib */
#endif /* EXAMPLE_HPP_ */
