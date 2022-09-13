#include <cstring>

#include "Board.hpp"
#include "GomokuException.hpp"

/*
** CONSTRUCTORS AND DESTRUCTORS
*/

Board::Board(size_t side) :
  _field(new t_point[side * side]),
  _score(0),
  _side(side)
{
    memset(_field, 0, sizeof(t_point) * _side * _side);
}

Board::Board(Board const & src)
  : _field(new t_point[src._side * src._side]),
  _score(src._score),
  _side(src._side)
{
  memcpy(_field, src._field, sizeof(t_point) * _side * _side);
}

Board::Board(Board && src) noexcept
{
  *this = std::move(src);
}

Board::~Board()
{
  delete [] _field;
}

/*
** OPERATORS
*/

Board&  Board::operator=(Board const & src)
{
  if (this == &src)
    return *this;
  Board temp(src);
  temp.swap(*this);
  return *this;
}

Board&  Board::operator=(Board && src) noexcept
{
  if (this == &src)
    return *this;
  this->swap(src);
  return (*this);
}

bool  Board::operator==(Board const & n) const
{
  return !std::memcmp(_field, n._field, sizeof(t_point) * _side * _side);
}

bool  Board::operator!=(Board const & n) const
{
  return !(*this == n);
}

bool  Board::operator<(Board const & n) const
{
  if (_score < n._score)
    return true;
  if (!(_score - n._score))
    return true;
  return false;
}

t_point&   Board::operator()(size_t x, size_t y)
{
  if (x >= _side && y >= _side)
    throw GomokuException(std::string(E_INDEX));
  return _field[y * _side + x];
}

t_point    Board::operator()(size_t x, size_t y) const
{
  if (x >= _side && y >= _side)
    throw GomokuException(std::string(E_INDEX));
  return _field[y * _side + x];
}

t_point&   Board::operator()(t_coord coord)
{
  if (coord.x >= _side && coord.y >= _side)
    throw GomokuException(std::string(E_INDEX));
  return _field[coord.y * _side + coord.x];
}

t_point    Board::operator()(t_coord coord) const
{
  if (coord.x >= _side && coord.y >= _side)
    throw GomokuException(std::string(E_INDEX));
  return _field[coord.y * _side + coord.x];
}


t_point&   Board::operator()(std::string pos)
{
  t_coord coord = pos_to_coord(pos);

  if (coord.x >= _side && coord.y >= _side)
    throw GomokuException(std::string(E_INDEX));
  return _field[coord.y * _side + coord.x];
}


t_point    Board::operator()(std::string pos) const
{
  t_coord coord = pos_to_coord(pos);

  if (coord.x >= _side && coord.y >= _side)
    throw GomokuException(std::string(E_INDEX));
  return _field[coord.y * _side + coord.x];
}


Board::operator bool()
{
  return _field;
}

/*
** SETTERS AND GETTERS
*/

void    Board::setScore(size_t score) { this->_score = score; }

/*
** UTILS METHODS
*/

void    Board::printBoard() const
{
  for (size_t y = 0; y != _side; ++y) {
    for (size_t x = 0; x != _side; ++x) {
      char p = '.';
      if ((*this)(x, y) == WHITE)
        p = 'W';
      else if ((*this)(x, y) == BLACK)
        p = 'B';
      printf("%-2c", p);
    }
    printf("\n");
  }
}

void    Board::swap(Board & src)
{
  std::swap(_score, src._score);
  std::swap(_field, src._field);
  std::swap(_side, src._side);
}

size_t    Board::getHash() const
{
  size_t hash = 0;
  size_t pow = 31;
  for (size_t i = 0; i != (_side * _side); ++i)
  {
    hash = hash + (_field[i] * pow);
    pow *= 31;
  }
  return hash;
}


t_coord     Board::pos_to_coord(std::string pos) const {
  t_coord coord = {
    (size_t)pos[0] - 97,
    _side - atoll(pos.substr(1UL).c_str())
  };
  return coord;
}


std::string Board::coord_to_pos(size_t x, size_t y) const {
  return std::string(1, char(97 + x)) + std::to_string(_side - y);
}


std::string Board::coord_to_pos(t_coord coord) const {
  return std::string(1, char(97 + coord.x)) + std::to_string(_side - coord.y);
}
