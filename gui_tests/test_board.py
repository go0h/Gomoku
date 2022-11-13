import pytest

from board import Board
from exceptions import BlackPlayerWinException
from color import Color

WHITE = "white"
BLACK = "black"


def get_board_with_pieces(pieces: dict, side=19):
    board = Board(board_size=side)
    set_pieces(board, pieces)
    return board


def set_pieces(board, pieces: dict):
    for pos, color in pieces.items():
        board.set_piece_by_pos(pos, color)


class TestBoard:

    # ##################################################################################
    # ############################## CHECK CONVERSIONS #################################
    # ##################################################################################
    def test_conversion_coordinates_to_position(self):
        b = Board(board_size=19)

        assert b.coordinates_to_position(0, 0) == "a19"
        assert b.coordinates_to_position(0, 18) == "a1"

        assert b.coordinates_to_position(18, 0) == "s19"
        assert b.coordinates_to_position(18, 18) == "s1"

        assert b.coordinates_to_position(5, 5) == "f14"
        assert b.coordinates_to_position(10, 8) == "k11"

        assert b.coordinates_to_position(7, 12) == "h7"
        assert b.coordinates_to_position(10, 8) == "k11"

        assert b.coordinates_to_position(14, 4) == "o15"
        assert b.coordinates_to_position(15, 15) == "p4"

    def test_conversion_position_to_coordinates(self):
        b = Board(board_size=19)

        assert b.position_to_coordinates("a19") == (0, 0)
        assert b.position_to_coordinates("a1") == (0, 18)

        assert b.position_to_coordinates("s19") == (18, 0)
        assert b.position_to_coordinates("s1") == (18, 18)

        assert b.position_to_coordinates("f14") == (5, 5)
        assert b.position_to_coordinates("k11") == (10, 8)

        assert b.position_to_coordinates("h7") == (7, 12)
        assert b.position_to_coordinates("k11") == (10, 8)

        assert b.position_to_coordinates("o15") == (14, 4)
        assert b.position_to_coordinates("p4") == (15, 15)

    # ##################################################################################
    # ################################### WINS #########################################
    # ##################################################################################
    def test_win_horizontal(self):
        piecies = {"g10": BLACK, "h10": BLACK, "i10": BLACK, "j10": BLACK, "k10": BLACK}
        board = get_board_with_pieces(piecies)

        coordinates = []
        for coord in (board.position_to_coordinates(x) for x in ["g10", "h10", "i10", "j10", "k10"]):
            coordinates.append(coord)

        with pytest.raises(BlackPlayerWinException) as exc:
            x, y = coordinates[4][0], coordinates[4][1]
            board.check_win_horizontals(x, y)
        assert str(exc.value) == 'Black Player Win'
        assert sorted(exc.value.win_coordinates) == sorted(coordinates)

        board.check_win_horizontals(x, y - 1)

        board.set_piece_by_pos("i10", WHITE)
        board.check_win_horizontals(x, y)

    def test_win_vertical(self):
        piecies = {"g10": BLACK, "g11": BLACK, "g12": BLACK, "g13": BLACK, "g14": BLACK}
        board = get_board_with_pieces(piecies)

        coordinates = []
        for coord in (board.position_to_coordinates(x) for x in ["g10", "g11", "g12", "g13", "g14"]):
            coordinates.append(coord)

        with pytest.raises(BlackPlayerWinException) as exc:
            x, y = coordinates[2][0], coordinates[2][1]
            board.check_win_verticals(x, y)
        assert str(exc.value) == 'Black Player Win'
        assert sorted(exc.value.win_coordinates) == sorted(coordinates)

        board.check_win_horizontals(x + 1, y)

        board.set_piece_by_pos("g12", WHITE)
        board.check_win_horizontals(x, y)

    def test_win_diagonal_top_left__bottom_right(self):
        piecies = {"a19": BLACK, "b18": BLACK, "c17": BLACK, "d16": BLACK, "e15": BLACK}
        board = get_board_with_pieces(piecies)

        coordinates = []
        for coord in (board.position_to_coordinates(x) for x in ["a19", "b18", "c17", "d16", "e15"]):
            coordinates.append(coord)

        with pytest.raises(BlackPlayerWinException) as exc:
            y, x = coordinates[3][0], coordinates[3][1]
            board.check_win_top_left__bottom_right(x, y)
        assert str(exc.value) == 'Black Player Win'
        assert sorted(exc.value.win_coordinates) == sorted(coordinates)

        board.check_win_top_left__bottom_right(x, y + 1)

        board.set_piece_by_pos("c17", WHITE)
        board.check_win_top_left__bottom_right(x, y)

    def test_win_diagonal_bottom_left__top_right(self):
        piecies = {"a1": BLACK, "b2": BLACK, "c3": BLACK, "d4": BLACK, "e5": BLACK}
        board = get_board_with_pieces(piecies)

        coordinates = []
        for coord in (board.position_to_coordinates(x) for x in ["a1", "b2", "c3", "d4", "e5"]):
            coordinates.append(coord)

        with pytest.raises(BlackPlayerWinException) as exc:
            x, y = coordinates[4][0], coordinates[4][1]
            board.check_win_top_right__bottom_left(x, y)
        assert str(exc.value) == 'Black Player Win'
        assert sorted(exc.value.win_coordinates) == sorted(coordinates)

        board.check_win_top_right__bottom_left(x, y - 1)

        board.set_piece_by_pos("c3", WHITE)
        board.check_win_top_right__bottom_left(x, y)

    def test_win_diagonal_top_right__bottom_left(self):
        piecies = {"s19": BLACK, "r18": BLACK, "q17": BLACK, "p16": BLACK, "o15": BLACK}
        board = get_board_with_pieces(piecies)

        coordinates = []
        for coord in (board.position_to_coordinates(x) for x in ["s19", "r18", "q17", "p16", "o15"]):
            coordinates.append(coord)

        with pytest.raises(BlackPlayerWinException) as exc:
            x, y = coordinates[3][0], coordinates[3][1]
            board.check_win_top_right__bottom_left(x, y)
        assert str(exc.value) == 'Black Player Win'
        assert sorted(exc.value.win_coordinates) == sorted(coordinates)

        board.check_win_top_right__bottom_left(x, y - 1)

        board.set_piece_by_pos("q17", WHITE)
        board.check_win_top_right__bottom_left(x, y)

    def test_win_diagonal_bottom_right__top_left(self):
        piecies = {"s1": BLACK, "r2": BLACK, "q3": BLACK, "p4": BLACK, "o5": BLACK}
        board = get_board_with_pieces(piecies)

        coordinates = []
        for coord in (board.position_to_coordinates(x) for x in ["s1", "r2", "q3", "p4", "o5"]):
            coordinates.append(coord)

        with pytest.raises(BlackPlayerWinException) as exc:
            y, x = coordinates[3][0], coordinates[3][1]
            board.check_win_top_left__bottom_right(x, y)
        assert str(exc.value) == 'Black Player Win'
        assert sorted(exc.value.win_coordinates) == sorted(coordinates)

        board.check_win_top_left__bottom_right(x, y + 1)

        board.set_piece_by_pos("q3", WHITE)
        board.check_win_top_left__bottom_right(x, y)

    # ##################################################################################
    # ################################### captureES ######################################
    # ##################################################################################
    def test_horizontal_capture_right(self):
        piecies = {"a19": BLACK, "b19": WHITE, "c19": WHITE}
        board = get_board_with_pieces(piecies)

        assert board.get_positions_of_captures("d19", BLACK) == ["c19", "b19"]
        assert board.get_positions_of_captures("d19", WHITE) == []

    def test_horizontal_capture_left(self):
        piecies = {"b19": WHITE, "c19": WHITE, "d19": BLACK}
        board = get_board_with_pieces(piecies)

        assert board.get_positions_of_captures("a19", BLACK) == ["b19", "c19"]
        assert board.get_positions_of_captures("a19", WHITE) == []

    def test_horizontal_bad_left(self):
        piecies = {"b19": WHITE, "c19": BLACK, "d19": BLACK}
        board = get_board_with_pieces(piecies)

        assert board.get_positions_of_captures("a19", BLACK) == []
        assert board.get_positions_of_captures("a19", WHITE) == []

    def test_vertical_capture_top(self):
        piecies = {"a1": BLACK, "a2": WHITE, "a3": WHITE}
        board = get_board_with_pieces(piecies)

        assert board.get_positions_of_captures("a4", BLACK) == ["a3", "a2"]
        assert board.get_positions_of_captures("a4", WHITE) == []

    def test_vertical_capture_bottom(self):
        piecies = {"a2": WHITE, "a3": WHITE, "a4": BLACK}
        board = get_board_with_pieces(piecies)

        assert board.get_positions_of_captures("a1", BLACK) == ["a2", "a3"]
        assert board.get_positions_of_captures("a1", WHITE) == []

    def test_diagonal_capture_top_left(self):
        piecies = {"b18": WHITE, "c17": WHITE, "d16": BLACK}
        board = get_board_with_pieces(piecies)

        assert board.get_positions_of_captures("a19", BLACK) == ["b18", "c17"]
        assert board.get_positions_of_captures("a19", WHITE) == []

    def test_diagonal_capture_bottom_left(self):
        piecies = {"b2": WHITE, "c3": WHITE, "d4": BLACK}
        board = get_board_with_pieces(piecies)

        assert board.get_positions_of_captures("a1", BLACK) == ["b2", "c3"]
        assert board.get_positions_of_captures("a1", WHITE) == []

    def test_diagonal_capture_top_right(self):
        piecies = {"r18": WHITE, "q17": WHITE, "p16": BLACK}
        board = get_board_with_pieces(piecies)

        assert board.get_positions_of_captures("s19", BLACK) == ["r18", "q17"]
        assert board.get_positions_of_captures("s19", WHITE) == []

    def test_diagonal_capture_bottom_right(self):
        piecies = {"r2": WHITE, "q3": WHITE, "p4": BLACK}
        board = get_board_with_pieces(piecies)

        assert board.get_positions_of_captures("s1", BLACK) == ["r2", "q3"]
        assert board.get_positions_of_captures("s1", WHITE) == []

    def test_all_captures_in_one(self):
        piecies = {"f14": BLACK, "g13": WHITE, "f13": BLACK, "h12": WHITE, "h14": BLACK, "i12": WHITE, "i14": BLACK,
                   "i13": WHITE, "k14": BLACK, "j12": WHITE, "l14": BLACK, "k13": WHITE, "l12": BLACK, "j11": WHITE,
                   "l11": BLACK, "k11": WHITE, "l9": BLACK, "j10": WHITE, "l8": BLACK, "k9": WHITE, "j8": BLACK,
                   "i10": WHITE, "i8": BLACK, "i9": WHITE, "g8": BLACK, "h10": WHITE, "f8": BLACK, "g9": WHITE,
                   "f10": BLACK, "h11": WHITE, "f11": BLACK, "g11": WHITE}
        board = get_board_with_pieces(piecies)

        result = sorted([key for key, value in piecies.items() if value == WHITE])

        assert sorted(board.get_positions_of_captures("i11", BLACK)) == result
        assert board.get_positions_of_captures("i11", WHITE) == []

    # ##################################################################################
    # ############################### FORBIDDEN_MOVES ##################################
    # ##################################################################################

    def test_horizontal_double_three(self):
        # _BB_{B}_BB_
        # _WW_{W}_WW_
        piecies = {"b11": BLACK, "c11": BLACK, "g11": BLACK, "h11": BLACK,
                   "b12": WHITE, "c12": WHITE, "g12": WHITE, "h12": WHITE}
        board = get_board_with_pieces(piecies)

        assert board.is_forbidden_turn_pos("e11", BLACK)
        assert board.is_forbidden_turn_pos("e12", WHITE)

        assert not board.is_forbidden_turn_pos("f11", BLACK)
        assert not board.is_forbidden_turn_pos("f12", WHITE)

        assert not board.is_forbidden_turn_pos("d11", BLACK)
        assert not board.is_forbidden_turn_pos("d12", WHITE)

    def test_vertical_double_three(self):
        piecies = {"e18": BLACK, "e17": BLACK, "e13": BLACK, "e12": BLACK,
                   "f18": WHITE, "f17": WHITE, "f13": WHITE, "f12": WHITE}
        board = get_board_with_pieces(piecies)

        assert board.is_forbidden_turn_pos("e15", BLACK)
        assert board.is_forbidden_turn_pos("f15", WHITE)

        assert not board.is_forbidden_turn_pos("e14", BLACK)
        assert not board.is_forbidden_turn_pos("f14", WHITE)

        assert not board.is_forbidden_turn_pos("e16", BLACK)
        assert not board.is_forbidden_turn_pos("f16", WHITE)

    def test_diagonal_top_left__bottom_right_double_three(self):
        piecies = {"b17": BLACK, "c16": BLACK, "g12": BLACK, "h11": BLACK,
                   "c18": WHITE, "d17": WHITE, "h13": WHITE, "i12": WHITE}
        board = get_board_with_pieces(piecies)

        assert board.is_forbidden_turn_pos("e14", BLACK)
        assert board.is_forbidden_turn_pos("f15", WHITE)

        assert not board.is_forbidden_turn_pos("d15", BLACK)
        assert not board.is_forbidden_turn_pos("e16", WHITE)

        assert not board.is_forbidden_turn_pos("f13", BLACK)
        assert not board.is_forbidden_turn_pos("g14", WHITE)

    def test_diagonal_bottom_left__top_right_double_three(self):
        piecies = {"b3": BLACK, "c4": BLACK, "g8": BLACK, "h9": BLACK,
                   "c2": WHITE, "d3": WHITE, "h7": WHITE, "i8": WHITE}

        board = get_board_with_pieces(piecies)

        assert board.is_forbidden_turn_pos("e6", BLACK)
        assert board.is_forbidden_turn_pos("f5", WHITE)

        assert not board.is_forbidden_turn_pos("d5", BLACK)
        assert not board.is_forbidden_turn_pos("e4", WHITE)

        assert not board.is_forbidden_turn_pos("f7", BLACK)
        assert not board.is_forbidden_turn_pos("g6", WHITE)

    def test_forbidden_from_subject(self):
        piecies = {"f14": BLACK, "g15": WHITE, "g13": BLACK, "f13": WHITE,
                   "j11": BLACK, "j13": WHITE, "k11": BLACK, "h10": WHITE}

        board = get_board_with_pieces(piecies)

        assert board.is_forbidden_turn_pos("i11", BLACK)

        assert not board.is_forbidden_turn_pos("h12", BLACK)
        assert not board.is_forbidden_turn_pos("l11", WHITE)
        assert not board.is_forbidden_turn_pos("e15", BLACK)

    def test_forbidden_diagonal_horizontal(self):
        piecies = {"g9": BLACK, "i9": WHITE, "h10": BLACK, "k10": WHITE,
                   "k12": BLACK, "h12": WHITE, "l12": BLACK, "l13": WHITE}

        board = get_board_with_pieces(piecies)

        assert board.is_forbidden_turn_pos("j12", BLACK)

        assert not board.is_forbidden_turn_pos("i11", BLACK)
        assert not board.is_forbidden_turn_pos("m12", WHITE)
        assert not board.is_forbidden_turn_pos("f8", BLACK)

    def test_forbidden_doubtful_case(self):
        """VERY DOUBTFUL CASE"""
        piecies = {"i10": BLACK, "k10": BLACK, "n10": BLACK,
                   "i11": WHITE, "k11": WHITE, "n11": WHITE}

        board = get_board_with_pieces(piecies)

        assert board.is_forbidden_turn_pos("l10", BLACK)
        assert board.is_forbidden_turn_pos("l11", WHITE)

        assert not board.is_forbidden_turn_pos("j10", BLACK)
        assert not board.is_forbidden_turn_pos("j11", WHITE)

    def test_forbidden_turn_all_in_one(self):
        piecies = {"g15": BLACK, "f16": BLACK, "i15": BLACK, "i16": BLACK, "k15": BLACK, "l16": BLACK, "g13": BLACK,
                   "f13": BLACK, "g11": BLACK, "f10": BLACK, "i11": BLACK, "i10": BLACK, "k11": BLACK, "l10": BLACK,
                   "k13": BLACK, "l13": BLACK}

        board = get_board_with_pieces(piecies)

        assert board.is_forbidden_turn_pos("i13", BLACK)

        assert board.is_forbidden_turn_pos("g14", BLACK)
        assert board.is_forbidden_turn_pos("f14", BLACK)
        assert board.is_forbidden_turn_pos("f15", BLACK)

        assert board.is_forbidden_turn_pos("f12", BLACK)
        assert board.is_forbidden_turn_pos("f11", BLACK)
        assert board.is_forbidden_turn_pos("g12", BLACK)

        assert board.is_forbidden_turn_pos("h11", BLACK)
        assert board.is_forbidden_turn_pos("h10", BLACK)
        assert board.is_forbidden_turn_pos("g10", BLACK)

        assert board.is_forbidden_turn_pos("j11", BLACK)
        assert board.is_forbidden_turn_pos("j10", BLACK)
        assert board.is_forbidden_turn_pos("k10", BLACK)

        assert board.is_forbidden_turn_pos("k12", BLACK)
        assert board.is_forbidden_turn_pos("l12", BLACK)
        assert board.is_forbidden_turn_pos("l11", BLACK)

        assert board.is_forbidden_turn_pos("k14", BLACK)
        assert board.is_forbidden_turn_pos("l15", BLACK)
        assert board.is_forbidden_turn_pos("l14", BLACK)

        assert board.is_forbidden_turn_pos("j16", BLACK)
        assert board.is_forbidden_turn_pos("j15", BLACK)
        assert board.is_forbidden_turn_pos("k16", BLACK)

        assert board.is_forbidden_turn_pos("h14", BLACK)
        assert board.is_forbidden_turn_pos("j14", BLACK)
        assert board.is_forbidden_turn_pos("h12", BLACK)
        assert board.is_forbidden_turn_pos("j12", BLACK)

        assert not board.is_forbidden_turn_pos("e17", BLACK)
        assert not board.is_forbidden_turn_pos("i17", WHITE)
        assert not board.is_forbidden_turn_pos("m17", BLACK)

    def test_double_free_after_capture(self):
        piecies = {"f14": BLACK, "g14": BLACK,
                   "h14": WHITE, "h15": WHITE,
                   "j14": WHITE, "j15": WHITE,
                   "k14": BLACK, "l14": BLACK,
                   "i14": BLACK,
                   "h16": BLACK, "j16": BLACK}

        board = get_board_with_pieces(piecies)

        assert not board.is_forbidden_turn_pos("h13", BLACK)
        assert not board.is_forbidden_turn_pos("j13", BLACK)

        assert board.get_positions_of_captures("h13", BLACK) == ["h14", "h15"]
        assert board.get_positions_of_captures("j13", BLACK) == ["j14", "j15"]

    def test_possible_horizontal_capture_left(self):

        piecies = {"a9": BLACK, "b9": WHITE, "d9": BLACK}

        board = get_board_with_pieces(piecies)

        assert board.is_possible_capture_pos("c9", WHITE)
        assert not board.is_possible_capture_pos("c9", BLACK)

        piecies = {"a9": BLACK, "c9": WHITE, "d9": BLACK}
        board = get_board_with_pieces(piecies)

        assert board.is_possible_capture_pos("b9", WHITE)
        assert not board.is_possible_capture_pos("b9", BLACK)

    def test_possible_horizontal_capture_right(self):

        piecies = {"s9": BLACK, "r9": WHITE, "p9": BLACK}

        board = get_board_with_pieces(piecies)

        assert board.is_possible_capture_pos("q9", WHITE)
        assert not board.is_possible_capture_pos("q9", BLACK)

        piecies = {"s9": BLACK, "q9": WHITE, "p9": BLACK}
        board = get_board_with_pieces(piecies)

        assert board.is_possible_capture_pos("r9", WHITE)
        assert not board.is_possible_capture_pos("r9", BLACK)

    def test_possible_vertical_capture_top(self):

        piecies = {"a19": BLACK, "a18": WHITE, "a16": BLACK}

        board = get_board_with_pieces(piecies)

        assert board.is_possible_capture_pos("a17", WHITE)
        assert not board.is_possible_capture_pos("a17", BLACK)

        piecies = {"a19": BLACK, "a17": WHITE, "a16": BLACK}
        board = get_board_with_pieces(piecies)

        assert board.is_possible_capture_pos("a18", WHITE)
        assert not board.is_possible_capture_pos("a18", BLACK)

    def test_possible_vertical_capture_bottom(self):

        piecies = {"a1": BLACK, "a2": WHITE, "a4": BLACK}

        board = get_board_with_pieces(piecies)

        assert board.is_possible_capture_pos("a3", WHITE)
        assert not board.is_possible_capture_pos("a3", BLACK)

        piecies = {"a1": BLACK, "a3": WHITE, "a4": BLACK}
        board = get_board_with_pieces(piecies)

        assert board.is_possible_capture_pos("a2", WHITE)
        assert not board.is_possible_capture_pos("a2", BLACK)

    def test_possible_diagonal_capture_top_left(self):

        piecies = {"a19": BLACK, "b18": WHITE, "d16": BLACK}

        board = get_board_with_pieces(piecies)

        assert board.is_possible_capture_pos("c17", WHITE)
        assert not board.is_possible_capture_pos("c17", BLACK)

        piecies = {"a19": BLACK, "c17": WHITE, "d16": BLACK}
        board = get_board_with_pieces(piecies)

        assert board.is_possible_capture_pos("b18", WHITE)
        assert not board.is_possible_capture_pos("b18", BLACK)

    def test_possible_diagonal_capture_bottom_right(self):

        piecies = {"s1": BLACK, "r2": WHITE, "p4": BLACK}

        board = get_board_with_pieces(piecies)

        assert board.is_possible_capture_pos("q3", WHITE)
        assert not board.is_possible_capture_pos("q3", BLACK)

        piecies = {"s1": BLACK, "q3": WHITE, "p4": BLACK}
        board = get_board_with_pieces(piecies)

        assert board.is_possible_capture_pos("r2", WHITE)
        assert not board.is_possible_capture_pos("r2", BLACK)

    def test_possible_diagonal_capture_bottom_left(self):

        piecies = {"a1": BLACK, "b2": WHITE, "d4": BLACK}

        board = get_board_with_pieces(piecies)

        assert board.is_possible_capture_pos("c3", WHITE)
        assert not board.is_possible_capture_pos("c3", BLACK)

        piecies = {"a1": BLACK, "c3": WHITE, "d4": BLACK}
        board = get_board_with_pieces(piecies)

        assert board.is_possible_capture_pos("b2", WHITE)
        assert not board.is_possible_capture_pos("b2", BLACK)

    def test_possible_diagonal_capture_top_right(self):

        piecies = {"s19": BLACK, "r18": WHITE, "p16": BLACK}

        board = get_board_with_pieces(piecies)

        assert board.is_possible_capture_pos("q17", WHITE)
        assert not board.is_possible_capture_pos("q17", BLACK)

        piecies = {"s19": BLACK, "q17": WHITE, "p16": BLACK}
        board = get_board_with_pieces(piecies)

        assert board.is_possible_capture_pos("r18", WHITE)
        assert not board.is_possible_capture_pos("r18", BLACK)

    def test_all_positions(self):
        board = Board(board_size=19)
        for y in range(board.board_size):
            for x in range(board.board_size):
                board.set_piece(x, y, Color.WHITE)
                board.check_win(x, y)
                board.set_piece(x, y, Color.EMPTY)

    def test_get_area(self):
        piecies = {"f8": WHITE, "g8": BLACK, "h8": BLACK}

        board = get_board_with_pieces(piecies)

        a = [(board.position_to_coordinates(p)) for p in ["g8", "h8"]]
        res = [board.coordinates_to_position(x, y) for (x, y) in board.create_area_of_win_strike(a)]

        assert sorted(res) == sorted(["e10", "f10", "g10", "h10", "i10", "j10",
                                       "f9", "g9", "h9", "i9",
                                       "i8", "j8",
                                       "f7", "g7", "h7", "i7",
                                       "e6", "f6", "g6", "h6", "i6", "j6"])
