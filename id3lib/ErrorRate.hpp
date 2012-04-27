/*
 * ErrorRate.hpp
 */

#ifndef ERRORRATE_HPP_
#define ERRORRATE_HPP_

namespace id3lib {

class ErrorRate {
public:
	ErrorRate() : misclassified(0.0f), all(0.0f) {

	}
	void operator+=(const ErrorRate& er) {
		misclassified += er.misclassified;
		all += er.all;
	}
	bool operator<(const ErrorRate& er) {
		return (misclassified / all) < (er.misclassified / er.all);
	}
	bool operator>(const ErrorRate& er) {
		return (misclassified / all) > (er.misclassified / er.all);
	}
	float misclassified;
	float all;
};

} /* namespace id3lib */
#endif /* ERRORRATE_HPP_ */
