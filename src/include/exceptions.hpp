#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <stdexcept>

namespace crossword {
    class CrosswordException : public std::runtime_error {
        public:
        CrosswordException(const std::string& message) : std::runtime_error(message) {}
    };

    class BasicException : public CrosswordException {
        public:
        BasicException(const std::string& message) : CrosswordException(message) {}
    };

    class CriticalException : public CrosswordException {
        public:
        CriticalException(const std::string& message) : CrosswordException(message) {}
    };
}

#endif // EXCEPTIONS_HPP