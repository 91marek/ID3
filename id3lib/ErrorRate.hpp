/*
 * ErrorRate.hpp
 */

#ifndef ERRORRATE_HPP_
#define ERRORRATE_HPP_

namespace id3lib {

class ErrorRate {
public:
	ErrorRate() : misclassified_(0.0f), all_(0.0f) {

	}
	/*
	 * @return Wartosc bledu
	 */
	float get() const {
		return misclassified_ / all_;
	}
	/*
	 * Funkcje nalezy wywolac, gdy napotkany przyklad jest
	 * blednie klasyfikowany
	 */
	void misclassifiedExample() {
		++misclassified_;
		++all_;
	}
	/*
	 * Funkcje nalezy wywolac, gdy napotkany przyklad jest
	 * poprawnie klasyfikowany
	 */
	void correctlyClassifiedExample() {
		++all_;
	}
	/*
	 * Zwieksza blad o inny blad
	 */
	void operator+=(const ErrorRate& er) {
		misclassified_ += er.misclassified_;
		all_ += er.all_;
	}
	/*
	 * Porownuje bledy
	 */
	bool operator<(const ErrorRate& er) const {
		return (misclassified_ / all_) < (er.misclassified_ / er.all_);
	}
	/*
	 * Porownuje bledy
	 */
	bool operator>(const ErrorRate& er) const {
		return (misclassified_ / all_) > (er.misclassified_ / er.all_);
	}
private:
	/*
	 * Liczba blednie klasyfikowanych przykladow
	 */
	float misclassified_;
	/*
	 * Liczba wszystkich przykladow
	 */
	float all_;
};

} /* namespace id3lib */
#endif /* ERRORRATE_HPP_ */
