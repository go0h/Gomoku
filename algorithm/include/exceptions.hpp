#ifndef EXCEPTIONS_H_
# define EXCEPTIONS_H_

# include <exception>
# include <cerrno>
# include <cstring>


struct InvalidArgument : public std::exception {
  virtual const char* what() const throw() {
    return "Invalid argument(s)";
  }
};

#endif // EXCEPTIONS_H_
