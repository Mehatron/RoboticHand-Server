#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <exception>
#include <string>
#include <ostream>

class Exception : public std::exception
{
public:
    Exception(const std::string &message, int code = 0)
        : m_message(message), m_code(code) {};
    virtual ~Exception(void) {};

    inline virtual const char *what(void) const throw() { return m_message.c_str(); };
    inline virtual std::string message(void) const { return m_message; };
    inline virtual int code(void) const { return m_code; };

private:
    std::string m_message;
    int m_code;
};

inline std::ostream &operator<<(std::ostream &lhs, const Exception &rhs)
{
    lhs << "[" << rhs.code() << "]: " << rhs.what() << std::endl;
    return lhs;
}

#endif // _EXCEPTION_H_
