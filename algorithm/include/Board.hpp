
#ifndef BOARD_HPP_
# define BOARD_HPP_

# include <string>

typedef enum s_color {
  EMPTY,
  WHITE,
  BLACK
}            t_color;

using t_point  = unsigned char;


class Board
{

public:

  Board() = default;
  explicit Board(t_point side);
  Board(Board const & src);
  Board(Board && src) noexcept ;
  ~Board();


  Board&      operator=(Board const & src);
  Board&      operator=(Board && src) noexcept ;
  t_point&    operator()(size_t i, size_t j);
  t_point     operator()(size_t i, size_t j) const;
  bool        operator==(Board const & n) const;
  bool        operator!=(Board const & n) const;
  bool        operator<(Board const & n) const;
  explicit    operator bool();


  unsigned    getScore() const { return _score; };
  t_point*    getField() const { return _field; };
  unsigned    getSide() { return _side; };
  void        setScore(size_t score);
  void        printBoard() const;
  size_t      getHash() const;

private:

  void         swap(Board & src);

  t_point*      _field = nullptr;
  unsigned      _score = 0;
  size_t        _side;
};

/*
**  HASHBoard
**  for STL's unordered_map/unordered_set
*/
class hashBoard {
public:
  size_t operator()(Board const & puzzle) const
  {
    return puzzle.getHash();
  }
};

#endif // BOARD_HPP_
