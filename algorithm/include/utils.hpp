
#ifndef UTILS_HPP_
# define UTILS_HPP_

# define BOOST_BIND_NO_PLACEHOLDERS

# include <iostream>
# include <sstream>
# include <string>
# include <boost/asio.hpp>
# include <boost/thread.hpp>
# include <boost/bind.hpp>


// colors for beauty
# define RED     "\033[0;31m"
# define YELLOW  "\033[0;33m"
# define GREEN   "\033[0;32m"
# define MAGENTA "\033[0;35m"
# define BLUE    "\033[0;34m"
# define CYAN    "\033[1;96m"
# define RESET   "\033[0m"

template<class Exception>
void  throw_if_true(bool result)  {
  if (result)
    throw Exception();
}

void fill_moves(unsigned board_size);
std::string get_random_move();
std::string get_and_remove_random_move();
void remove_move(std::string move);


#endif  // UTILS_HPP_
