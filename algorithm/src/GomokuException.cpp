
#include "GomokuException.hpp"

GomokuException::GomokuException(const std::string & err)
{
  error_ = "Error: " + err;
}

std::string GomokuException::what() noexcept
{
  return error_;
}
