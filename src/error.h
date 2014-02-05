#ifndef ERROR_H
#define ERROR_H

#include <exception>

class QString;

namespace MyNotes {

class Error : public std::exception {
  public:
    explicit Error(const QString &message) throw()
        : message(message.toUtf8()) {}
    ~Error() throw() {}

    const char *what() const throw() { return message; }

  private:
    const char *message;
};
}
#endif // ERROR_H
