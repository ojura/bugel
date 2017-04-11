#ifndef BUGELEXCEPTION
#define BUGELEXCEPTION

#include <QString>

#include <string>
#include <sstream>
#include <exception>

// shitty exception class
// why doesn't qt have something like this default
// god damn it shit fuck shit fuck fuck shit

class BugelException : public std::exception
{
public:
    std::string msg;
    const char* what() const throw() { return msg.c_str(); }
};

template<typename T>
BugelException& operator<<(BugelException&& e, const T appendMsg)
{
    std::stringstream ss;
    ss << e.msg << appendMsg;
    e.msg = ss.str().c_str();
    return e;
}

template<typename T>
BugelException& operator<<(BugelException& e, const T appendMsg)
{
    return std::move(e) << appendMsg;
}

std::ostream& operator<<(std::ostream& stream, const QString& str);

// sub-exceptions
class EventException : public BugelException {};
class ScriptException : public BugelException {};

#endif // BUGELEXCEPTION

