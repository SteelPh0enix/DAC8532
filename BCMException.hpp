#ifndef BCMEXCEPTION_HPP
#define BCMEXCEPTION_HPP
#include <exception>

class BCMInitException : public std::exception {
public:
    virtual ~BCMInitException() = default;
    virtual const char* what() const noexcept {
        return "Broadcom 2835 failed to initialize!";
    }
};

#endif