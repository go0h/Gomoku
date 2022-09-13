
#ifndef GOMOKU_EXCEPTION_HPP
# define GOMOKU_EXCEPTION_HPP

#define USAGE        "Usage: ./Npuzzle puzzle_file [OPTION]... [FILE]..."
#define E_INDEX      "Incorrect indexes"

#include <exception>
#include <string>

class GomokuException : public std::exception
{
private:
  std::string error_;
public:
  explicit GomokuException(const std::string & err);
  std::string what() noexcept;
};

#endif
