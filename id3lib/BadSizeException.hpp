/*
 * BadSizeException.hpp
 */

#ifndef BADSIZEEXCEPTION_HPP_
#define BADSIZEEXCEPTION_HPP_

#include <exception>

namespace id3lib {

/*
 * Wyjatek oznaczajacy zly rozmiar parametru
 */
class BadSizeException: public std::exception {

};

} /* namespace id3lib */
#endif /* BADSIZEEXCEPTION_HPP_ */