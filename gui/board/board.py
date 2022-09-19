import typing as t
from typing import List, Tuple

from constants import LETTERS, LENGTH_WIN_SEQUENCE
from color import Color
from exceptions import ConfigGomokuError, BlackPlayerWinException, WhitePlayerWinException, BusyCell

# THREE PATTERNS
# _XXX_
B_1 = [Color.EMPTY, Color.BLACK, Color.BLACK, Color.BLACK, Color.EMPTY]
W_1 = [Color.EMPTY, Color.WHITE, Color.WHITE, Color.WHITE, Color.EMPTY]

# _X_XX_
B_2 = [Color.EMPTY, Color.BLACK, Color.EMPTY, Color.BLACK, Color.BLACK, Color.EMPTY]
W_2 = [Color.EMPTY, Color.WHITE, Color.EMPTY, Color.WHITE, Color.WHITE, Color.EMPTY]

# _XX_X_
B_3 = [Color.EMPTY, Color.BLACK, Color.BLACK, Color.EMPTY, Color.BLACK, Color.EMPTY]
W_3 = [Color.EMPTY, Color.WHITE, Color.WHITE, Color.EMPTY, Color.WHITE, Color.EMPTY]

BLACK_PATTERNS = [B_1, B_2, B_3]
WHITE_PATTERNS = [W_1, W_2, W_3]

BLACK_CATCH_1 = [Color.BLACK, Color.WHITE, Color.EMPTY, Color.BLACK]
BLACK_CATCH_2 = [Color.BLACK, Color.EMPTY, Color.WHITE, Color.BLACK]
WHITE_CATCH_1 = [Color.WHITE, Color.EMPTY, Color.BLACK, Color.WHITE]
WHITE_CATCH_2 = [Color.WHITE, Color.BLACK, Color.EMPTY, Color.WHITE]

BLACK_CATCH = [BLACK_CATCH_1, BLACK_CATCH_2]
WHITE_CATCH = [WHITE_CATCH_1, WHITE_CATCH_2]


class Board:

    def __init__(self, **kwargs):
        self.board_size = kwargs.get('board_size')
        self.board = [[Color.EMPTY for _ in range(self.board_size)] for _ in range(self.board_size)]
        self.check_init()

    def check_init(self) -> None:
        if self.board_size is None:
            raise ConfigGomokuError('Board_size of Gomoku is unfilled')

    def set_piece(self, x: int, y: int, color: Color):
        self.board[y][x] = color

    def set_piece_by_pos(self, position: t.Union[str, t.Iterable], color: str):
        if isinstance(position, str):
            x, y = self.position_to_coordinates(position)
        elif isinstance(position, (tuple, list, set)) and len(position) == 2:
            x, y = position[0], position[1]
        else:
            raise TypeError
        c = Color.WHITE if color.upper() == Color.WHITE.name else Color.BLACK
        self.set_piece(x, y, c)

    def delete_piece(self, x: int, y: int):
        self.board[y][x] = Color.EMPTY

    def delete_piece_by_pos(self, position: t.Union[str, t.Iterable]):
        if isinstance(position, str):
            x, y = self.position_to_coordinates(position)
        elif isinstance(position, (tuple, list, set)) and len(position) == 2:
            x, y = position[0], position[1]
        else:
            raise TypeError
        self.delete_piece(x, y)

    def coordinates_to_position(self, x: int, y: int) -> str:
        if x > self.board_size or y > self.board_size:
            raise ValueError(f'Coordinate ({x}, {y}) absence on board')
        return LETTERS[x] + str(self.board_size - y)

    def position_to_coordinates(self, position: str) -> t.Tuple[int, int]:
        # if re.match('[a-z]', coord[0]) is None or re.match('^\d{1:2}$', coord[1:]) is None:
        #     raise ValueError(f'Coordinate {coord} is invalid')
        x = LETTERS.find(position[0])
        y = self.board_size - int(position[1:])
        if x > self.board_size or y > self.board_size:
            raise ValueError(f'Coordinate {position} absence on board')
        return x, y

    def checks(self, x: int, y: int):
        self.check_free_pos(x, y)
        self.check_win(x, y)
        # self.check_forbidden_turn(x, y)

    def check_free_pos(self, x: int, y: int):
        if self.board[y][x] != Color.EMPTY:
            raise BusyCell(x, y)

    def check_win(self, x: int, y: int):
        self.check_win_horizontals(x, y)
        self.check_win_verticals(x, y)
        self.check_win_top_left__bottom_right(x, y)
        self.check_win_top_right__bottom_left(x, y)

    def get_win_strike(self, position: str):
        x, y = self.position_to_coordinates(position)
        try:
            self.check_win(x, y)
        except (WhitePlayerWinException, BlackPlayerWinException) as exc:
            return [self.coordinates_to_position(x, y) for x, y in exc.win_coordinates]
        return []

    def check_win_horizontals(self, x: int, y: int):
        check_color = self.board[y][x]
        if check_color == Color.EMPTY:
            return
        seq_len = 0
        min_boarder = max(0, x - (LENGTH_WIN_SEQUENCE - 1))
        max_boarder = min(self.board_size, x + LENGTH_WIN_SEQUENCE)
        for x in range(min_boarder, max_boarder):
            if self.board[y][x] == check_color:
                seq_len += 1
            else:
                seq_len = 0
            if seq_len == LENGTH_WIN_SEQUENCE:
                exception_type = BlackPlayerWinException if check_color == Color.BLACK else WhitePlayerWinException
                exception = exception_type(coordinates=[(i, y) for i in range(x - LENGTH_WIN_SEQUENCE + 1, x + 1)])
                raise exception

    def check_win_verticals(self, x: int, y: int):
        check_color = self.board[y][x]
        if check_color == Color.EMPTY:
            return
        seq_len = 0
        min_boarder = max(0, y - (LENGTH_WIN_SEQUENCE - 1))
        max_boarder = min(self.board_size, y + LENGTH_WIN_SEQUENCE)
        for y in range(min_boarder, max_boarder):
            if self.board[y][x] == check_color:
                seq_len += 1
            else:
                seq_len = 0
            if seq_len == LENGTH_WIN_SEQUENCE:
                exception_type = BlackPlayerWinException if check_color == Color.BLACK else WhitePlayerWinException
                exception = exception_type(coordinates=[(x, i) for i in range(y - LENGTH_WIN_SEQUENCE + 1, y + 1)])
                raise exception

    def check_win_top_left__bottom_right(self, x: int, y: int):
        check_color = self.board[y][x]
        if check_color == Color.EMPTY:
            return
        distance_to_first_boarder = min(x, y)
        distance_to_second_boarder = min(self.board_size - x - 1, self.board_size - y - 1)
        distance_1 = min(distance_to_first_boarder, 4)
        distance_2 = min(distance_to_second_boarder, 5)
        seq_len = 0
        if distance_1 + distance_2 + 1 < LENGTH_WIN_SEQUENCE:
            return
        for i in range(-1 * distance_1, distance_2 + 1):
            if self.board[y + i][x + i] == check_color:
                seq_len += 1
            else:
                seq_len = 0
            if seq_len == LENGTH_WIN_SEQUENCE:
                exception_type = BlackPlayerWinException if check_color == Color.BLACK else WhitePlayerWinException
                exception = exception_type(coordinates=[(x + i - k, y + i - k)
                                                        for k in range(LENGTH_WIN_SEQUENCE)])
                raise exception

    def check_win_top_right__bottom_left(self, x: int, y: int):
        """Check diagonal top-right -> bottom-left"""
        check_color = self.board[y][x]
        if check_color == Color.EMPTY:
            return
        distance_to_first_boarder = min(self.board_size - x - 1, y)
        distance_to_second_boarder = min(x, self.board_size - y - 1)
        distance_1 = min(distance_to_first_boarder, 4)
        distance_2 = min(distance_to_second_boarder, 5)
        seq_len = 0
        if distance_1 + distance_2 + 1 < LENGTH_WIN_SEQUENCE:
            return
        for i in range(-1 * distance_1, distance_2 + 1):
            if self.board[y + i][x - i] == check_color:
                seq_len += 1
            else:
                seq_len = 0
            if seq_len == LENGTH_WIN_SEQUENCE:
                exception_type = BlackPlayerWinException if check_color == Color.BLACK else WhitePlayerWinException
                exception = exception_type(coordinates=[(x - i + k, y + i - k)
                                                        for k in range(LENGTH_WIN_SEQUENCE)])
                raise exception

    def get_coordinates_of_captures(self, position: str, color: str):
        x, y = self.position_to_coordinates(position)
        c = Color.WHITE if color.upper() == Color.WHITE.name else Color.BLACK
        return self.get_captures(x, y, c)

    def get_positions_of_captures(self, position: str, color: str) -> List[str]:
        """Returns list of captures pieces. [a1, a2]"""
        coordinates = self.get_coordinates_of_captures(position, color)
        return [self.coordinates_to_position(x, y) for x, y in coordinates]

    def _capture(self, x, y, x_dir, y_dir, color, versa_color):
        if self.board[y + y_dir * 3][x + x_dir * 3] == color and \
                self.board[y + y_dir][x + x_dir] == versa_color and \
                self.board[y + y_dir * 2][x + x_dir * 2] == versa_color:
            self.board[y + y_dir][x + x_dir] = Color.EMPTY
            self.board[y + y_dir * 2][x + x_dir * 2] = Color.EMPTY
            return [(x + x_dir, y + y_dir), (x + x_dir * 2, y + y_dir * 2)]
        return []

    def get_captures(self, x: int, y: int, color: Color) -> List[str]:
        """Returns list of captures pieces. [(x1, y1), (x2, y2), ..]"""
        captures = []
        versa_color = Color.WHITE if color == Color.BLACK else Color.BLACK

        # horizontal left <-> right
        if x > 2:
            captures += self._capture(x, y, -1, 0, color, versa_color)
        if x < self.board_size - 3:
            captures += self._capture(x, y, 1, 0, color, versa_color)

        # vertical top <-> bottom
        if y > 2:
            captures += self._capture(x, y, 0, -1, color, versa_color)
        if y < self.board_size - 3:
            captures += self._capture(x, y, 0, 1, color, versa_color)

        # diagonal top-left <-> bottom-right
        if x > 2 and y > 2:
            captures += self._capture(x, y, -1, -1, color, versa_color)
        if x < self.board_size - 3 and y < self.board_size - 3:
            captures += self._capture(x, y, 1, 1, color, versa_color)

        # diagonal top-right <-> bottom-left
        if x < self.board_size - 3 and y > 2:
            captures += self._capture(x, y, 1, -1, color, versa_color)
        if x > 2 and y < self.board_size - 3:
            captures += self._capture(x, y, -1, 1, color, versa_color)
        return captures

    def is_forbidden_turn_pos(self, pos: str, color: str) -> bool:
        """Returns False if after this move creates double-three (or more than double)"""
        x, y = self.position_to_coordinates(pos)
        c = Color.WHITE if color.upper() == Color.WHITE.name else Color.BLACK
        return self.is_forbidden_turn(x, y, c)

    def is_forbidden_turn(self, x: int, y: int, color: Color) -> bool:
        """Find free-three in all directions on board
           Return False if there is more than one free-three, or position in capture"""
        return self.get_num_of_free_threes(x, y, color) > 1 or self.is_possible_capture(x, y, color)

    def get_num_of_free_threes(self, x: int, y: int, color: Color) -> int:
        """Return number of free-threes"""
        if self.board[y][x] != Color.EMPTY:
            return 0
        self.board[y][x] = color
        three_num = 0

        # horizontal
        if 0 < x < self.board_size - 1:
            three_num += self._get_free_threes(x, y, 1, 0, color)

        # vertical
        if 0 < y < self.board_size - 1:
            three_num += self._get_free_threes(x, y, 0, 1, color)

        # diagonal top-left <-> bottom-right
        if 0 < x < self.board_size - 1 and 0 < y < self.board_size - 1:
            three_num += self._get_free_threes(x, y, 1, 1, color)

        # diagonal top-right <-> bottom-left
        if 0 < x < self.board_size - 1 and 0 < y < self.board_size - 1:
            three_num += self._get_free_threes(x, y, 1, -1, color)

        self.board[y][x] = Color.EMPTY
        return three_num

    def _get_free_threes(self, x: int, y: int, x_dir: int, y_dir: int, color: Color):
        three_num = 0
        pattern_list = BLACK_PATTERNS if color == Color.BLACK else WHITE_PATTERNS
        for pattern in pattern_list:
            for i in range(1, len(pattern) - 1):
                three_num += self._check_three(x - i * x_dir, y - i * y_dir, x_dir, y_dir, pattern)
        return three_num

    def _check_three(self, x, y, x_dir, y_dir, pattern):
        for i in range(len(pattern)):
            x_ = x + i * x_dir
            y_ = y + i * y_dir
            if -1 < x_ < self.board_size and -1 < y_ < self.board_size \
                    and self.board[y_][x_] == pattern[i]:
                continue
            else:
                return 0
        return 1

    def is_possible_capture_pos(self, pos: str, color: str):
        x, y = self.position_to_coordinates(pos)
        c = Color.WHITE if color.upper() == Color.WHITE.name else Color.BLACK
        return self.is_possible_capture(x, y, c)

    def is_possible_capture(self, x: int, y: int, color: Color):

        patterns = BLACK_CATCH if color == Color.WHITE else WHITE_CATCH

        # horizontal
        if self._is_patterns(x - 1, y, 1, 0, patterns) or self._is_patterns(x - 2, y, 1, 0, patterns):
            return True

        # # vertical
        if self._is_patterns(x, y - 1, 0, 1, patterns) or self._is_patterns(x, y - 2, 0, 1, patterns):
            return True

        # diagonal top-left -> bottom-right
        if self._is_patterns(x - 1, y - 1, 1, 1, patterns) or self._is_patterns(x - 2, y - 2, 1, 1, patterns):
            return True

        # diagonal top-right -> bottom-left
        if self._is_patterns(x + 1, y - 1, -1, 1, patterns) or self._is_patterns(x + 2, y - 2, -1, 1, patterns):
            return True
        return False

    def _is_patterns(self, x: int, y: int, x_dir: int, y_dir: int, patterns):
        for pattern in patterns:
            if self._is_pattern(x, y, x_dir, y_dir, pattern):
                return True
        return False

    def _is_pattern(self, x: int, y: int, x_dir: int, y_dir: int, pattern):
        for i in range(0, len(pattern)):
            x_ = x + i * x_dir
            y_ = y + i * y_dir
            if -1 < x_ < self.board_size and -1 < y_ < self.board_size \
                    and self.board[y_][x_] == pattern[i]:
                continue
            else:
                return False
        return True

    def get_num_of_captures_by_pos(self, pos_strike: List[str], color: str) -> bool:
        strike = [(self.position_to_coordinates(pos)) for pos in pos_strike]
        c = Color.WHITE if color.upper() == Color.WHITE.name else Color.BLACK
        return self.get_num_of_captures(strike, c)

    def get_num_of_captures(self, strike: List[Tuple[int, int]], color: Color) -> bool:
        versa_color = Color.BLACK if color == Color.WHITE else Color.WHITE
        area = self.create_area_of_win_strike(strike)
        num_captures = 0
        for x, y in area:
            if not self.is_forbidden_turn(x, y, versa_color):
                self.board[y][x] = versa_color
                captures = self.get_captures(x, y, versa_color)
                if len(captures) != 0:
                    for x_, y_ in captures:
                        self.board[y_][x_] = color
                    self.board[y][x] = Color.EMPTY
                    num_captures += len(captures)
                self.board[y][x] = Color.EMPTY
        return num_captures

    def create_area_of_win_strike(self, strike):
        area = []
        y_dir = strike[1][0] - strike[0][0]
        x_dir = strike[1][1] - strike[0][1]
        x_f, y_f = strike[0]
        x_l, y_l = strike[4]
        new_strike = [(x_f - 2 * y_dir, y_f - 2 * x_dir), (x_f - 1 * y_dir, y_f - 1 * x_dir)] \
            + strike + [(x_l + 2 * y_dir, y_l + 2 * x_dir), (x_l + 1 * y_dir, y_l + 1 * x_dir)]
        for i in range(len(new_strike)):
            for shift in [-2, -1, 1, 2]:
                res = self._x_y(new_strike[i][0], new_strike[i][1], x_dir, y_dir, shift)
                if res is not None:
                    area.append(res)
        return area

    def _x_y(self, x, y, x_dir, y_dir, shift):
        x_ = x + shift * x_dir
        y_ = y + shift * y_dir
        if 0 <= x_ < self.board_size and 0 <= y_ < self.board_size \
                and self.board[y_][x_] == Color.EMPTY:
            return x_, y_
        return None

    def __str__(self):
        res = ""
        for line in self.board:
            for pos in line:
                if pos == Color.EMPTY:
                    res += "- "
                elif pos == Color.WHITE:
                    res += "W "
                else:
                    res += "B "
            res += "\n"
        return res
