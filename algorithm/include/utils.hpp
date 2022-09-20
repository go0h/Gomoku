
#ifndef UTILS_HPP_
# define UTILS_HPP_

# define BOOST_BIND_NO_PLACEHOLDERS

# include <iostream>
# include <sstream>
# include <string>


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

int compare_moves_desc(const void* elem1, const void* elem2);

int compare_moves_asc(const void* elem1, const void* elem2);

#endif  // UTILS_HPP_
