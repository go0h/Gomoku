
#ifndef BOARD_HPP_
# define BOARD_HPP_

# include <string>

typedef enum   s_color {
  EMPTY,
  WHITE,
  BLACK
}              t_color;

typedef struct s_coord
{
  size_t x;
  size_t y;
}              t_coord;

using t_point  = unsigned char;


class Board
{

public:

  Board() = default;
  explicit Board(size_t side);
  Board(Board const & src);
  Board(Board && src) noexcept ;
  ~Board();


  Board&      operator=(Board const & src);
  Board&      operator=(Board && src) noexcept ;
  t_point&    operator()(size_t i, size_t j);
  t_point     operator()(size_t i, size_t j) const;
  t_point&    operator()(t_coord coord);
  t_point     operator()(t_coord coord) const;
  t_point&    operator()(std::string pos);
  t_point     operator()(std::string pos) const;
  bool        operator==(Board const & n) const;
  bool        operator!=(Board const & n) const;
  bool        operator<(Board const & n) const;
  explicit    operator bool();


  t_point*    getField() const { return _field; };
  size_t      getSide() { return _side; };
  void        setScore(size_t score);
  void        printBoard() const;
  size_t      getHash() const;

  t_coord     pos_to_coord(std::string pos) const;
  std::string coord_to_pos(size_t x, size_t y) const;
  std::string coord_to_pos(t_coord coord) const;


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
