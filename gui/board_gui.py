import time
import tkinter as ttk
import json
from player import Player
from piece import Piece
from board import Board

from constants import (
    PAD_FROM_WIN,
    BACKGROUND_COLOR, BOARD_COLOR,
    FONT, LABEL_FONT_SIZE, LABEL_FONT, WINNER_COLOR
)


class BoardGui:

    def __init__(self, win, config, send_func):
        self._win = win
        self._config = config

        # board props
        self._board_width = win.winfo_height() - PAD_FROM_WIN
        self._padding = 50
        self._cell_width = (self._board_width - 2 * self._padding) / (self._config.get_board_size() - 1)
        self._piece_radius = round(self._cell_width / 3)
        self._board_frame = None
        self._board_canvas = None
        self._board = Board(board_size=self._config.get_board_size())

        # info frame props
        self._info_frame = None
        self._turn = ttk.StringVar()
        self._moves = ttk.IntVar()
        self._moves.set(0)
        self._time_spend_label = None
        self._time_spend = time.perf_counter()
        self._winner = None
        self._winner_name = None
        self._win_strike = []

        # player properties
        self._p1 = None
        self._p2 = None
        self._cur_player = None
        self._pieces = []
        # p2: [piece1, piece2]
        self._captured = dict()
        self._hints = []

        self._send_func = send_func

    def print_board(self):
        """Initialize game board"""
        self._board_frame = ttk.Frame(self._win,
                                      width=self._win.winfo_height(),
                                      height=self._win.winfo_height(),
                                      bg=BACKGROUND_COLOR,
                                      bd=10)

        rel_x = 1 - (self._win.winfo_width() - self._win.winfo_height() / 2) / self._win.winfo_width()
        self._board_frame.place(relx=rel_x, rely=.5, anchor="center")

        self._board_canvas = ttk.Canvas(self._board_frame,
                                        width=self._board_width,
                                        height=self._board_width,
                                        bg=BOARD_COLOR)
        self._board_canvas.place(relx=.5, rely=.5, anchor="center")

        padding = self._padding
        width = self._board_width
        cell_width = self._cell_width

        for n in range(0, self._config.get_board_size()):
            # print lines
            self._board_canvas.create_line(padding, padding + cell_width * n, width - padding, padding + cell_width * n)
            self._board_canvas.create_line(padding + cell_width * n, padding, padding + cell_width * n, width - padding)

            # print lines
            self._board_canvas.create_text(padding / 2, width - padding - cell_width * n, text=str(n + 1))
            self._board_canvas.create_text(width - padding / 2, width - padding - cell_width * n, text=str(n + 1))

            # print lines
            self._board_canvas.create_text(padding + cell_width * n, padding / 2, text=chr(97 + n))
            self._board_canvas.create_text(padding + cell_width * n, width - padding / 2, text=chr(97 + n))

        self._board_canvas.bind('<Button-1>', self.make_turn_on_event)
        self._print_info()

    def _print_info(self):
        """Initialize info panel"""
        self._info_frame = ttk.Frame(self._win,
                                     width=self._win.winfo_width() - self._win.winfo_height() - PAD_FROM_WIN / 2,
                                     height=(self._win.winfo_height() / 4) * 3 - PAD_FROM_WIN,
                                     bg=BOARD_COLOR)
        self._info_frame.update()

        win_width = self._win.winfo_width()
        f_width = self._win.winfo_width() - self._win.winfo_height() - PAD_FROM_WIN / 2
        f_height = (self._win.winfo_height() / 4) * 3 - PAD_FROM_WIN

        rel_x = (win_width - (f_width + PAD_FROM_WIN) / 2) / win_width
        self._info_frame.place(relx=rel_x, y=f_height / 2 + (PAD_FROM_WIN / 2), anchor="center")

        info_l = ttk.Label(self._info_frame,
                           text='INFO',
                           font=(FONT, LABEL_FONT_SIZE, "bold"),
                           bg=BOARD_COLOR)
        info_l.place(relx=.5, rely=.05, anchor="center")

        font_u = [FONT, LABEL_FONT_SIZE, "underline"]

        mode_l = ttk.Label(self._info_frame, text='Mode:', font=font_u, bg=BOARD_COLOR)
        mode_l.place(relx=.1, rely=.19, anchor="w")

        mode = ttk.Label(self._info_frame, text=self._config.get_mode(), font=LABEL_FONT, bg=BOARD_COLOR)
        mode.place(relx=.7, rely=.19, anchor="center")

        moves_l = ttk.Label(self._info_frame, text='Moves:', font=font_u, bg=BOARD_COLOR)
        moves_l.place(relx=.1, rely=.33, anchor="w")

        moves = ttk.Label(self._info_frame, textvariable=self._moves, font=LABEL_FONT, bg=BOARD_COLOR)
        moves.place(relx=.7, rely=.33, anchor="center")

        turn_l = ttk.Label(self._info_frame, text='Turn:', font=font_u, bg=BOARD_COLOR)
        turn_l.place(relx=.1, rely=.47, anchor="w")

        turn = ttk.Label(self._info_frame, textvariable=self._turn, font=LABEL_FONT, bg=BOARD_COLOR)
        turn.place(relx=.7, rely=.47, anchor="center")

        time_l = ttk.Label(self._info_frame, text='Taken:', font=font_u, bg=BOARD_COLOR)
        time_l.place(relx=.1, rely=.61, anchor="w")

        self._time_spend_label = ttk.Label(self._info_frame, text="0.00", font=LABEL_FONT, bg=BOARD_COLOR)
        self._time_spend_label.place(relx=.7, rely=.61, anchor="center")

        captures_l = ttk.Label(self._info_frame, text='Captures:', font=font_u, bg=BOARD_COLOR)
        captures_l.place(relx=.1, rely=.75, anchor="w")

        if self._config.get_mode() == "PvE":
            if self._config.get_first_move() == "BLACK":
                self._p1 = Player(self._info_frame, "Player", "black")
                self._p2 = Player(self._info_frame, f"Bot {self._config.get_difficult().lower()}", "white")
            else:
                self._p1 = Player(self._info_frame, f"Bot {self._config.get_difficult().lower()}", "black")
                self._p2 = Player(self._info_frame, "Player", "white")
        else:
            self._p1 = Player(self._info_frame, "Player #1", "black")
            self._p2 = Player(self._info_frame, "Player #2", "white")
        self._cur_player = self._p1
        self._turn.set(self._cur_player.get_name())

        self._p1.place(rel_y=.85)
        self._p2.place(rel_y=.93)

    def _show_move_time(self):
        showtime = f"{time.perf_counter() - self._time_spend:0.2f}"
        self._time_spend_label.config(text=showtime)

    def _stop_move_time(self):
        self._time_spend_label.config(text="0.00")

    def print_winner(self, strike, **kwargs):
        self._winner = ttk.Label(self._info_frame, text="Winner:",
                                 font=[FONT, LABEL_FONT_SIZE, "underline", "bold"],
                                 bg=BOARD_COLOR)
        self._winner.place(relx=.1, rely=.47, anchor="w")

        self._winner_name = ttk.Label(self._info_frame, text=self._turn.get(),
                        font=[FONT, LABEL_FONT_SIZE, "underline", "bold"],
                        fg=WINNER_COLOR,
                        bg=BOARD_COLOR)
        self._winner_name.place(relx=.7, rely=.47, anchor="center")

        self.delete_hints()
        self._print_win_strike(strike)
        self._show_move_time()
        self._board_canvas.unbind('<Button-1>')
        self._moves.set(self._moves.get() + 1)

        self._send_func(method="winner", arguments={"winner": self._cur_player._color})

    def print_forbidden_move(self, x, y):
        p = BoardGui.create_circle(self._board_canvas, x, y, self._piece_radius, fill="red")
        time.sleep(.1)
        self._board_canvas.delete(p)

    def print_hints(self, **kwargs):
        for hint in kwargs.get("hints"):
            x, y = self.get_coordinates_on_board_by_pos(hint)
            h = self.create_circle(self._board_canvas, x, y, self._piece_radius * .4,
                                   fill="red", width=0)
            self._hints.append(h)

    def delete_hints(self):
        for _ in range(len(self._hints)):
            self._board_canvas.delete(self._hints.pop())

    def make_turn_on_event(self, event):
        """Event on-click on board to set Piece"""
        if self._cur_player.get_name().startswith("Bot"):
            return
        self.delete_hints()
        x = BoardGui.round_cell(event.x - self._padding, self._cell_width) + self._padding
        y = BoardGui.round_cell(event.y - self._padding, self._cell_width) + self._padding

        position = chr(ord('a') + round((x - self._padding) / self._cell_width)) + \
                   str(self._config.get_board_size() - round((y - self._padding) / self._cell_width))

        if self.if_pos_in_bound(x, y):

            if self._board.is_forbidden_turn_pos(position, self._cur_player.get_color()):
                self.print_forbidden_move(x, y)
                return

            if self.set_piece(position, x, y, self._cur_player.get_color()):
                capture = self._board.get_positions_of_captures(position, self._cur_player.get_color())
                if len(capture) != 0:
                    self.hide_captured(position, capture)
                    self._cur_player.add_captures(len(capture))

                strike = self._board.get_win_strike(position)
                # если есть выйгрышная серия и если противник не сможет захватить ее и набрать
                # за последний ход >= 10 захватов
                opponent = self._p2 if self._cur_player == self._p1 else self._p1
                if (len(strike) != 0 \
                        and (opponent.captures() + self._board.get_max_of_captures_by_pos(strike, self._cur_player.get_color()) < 10)) \
                    or self._cur_player.captures() >= 10:
                    self.print_winner(strike)
                else:
                    self._send_method("make_turn", position, capture)
                    self._next()

    def make_turn(self, position, **kwargs):
        if not self.piece_on_table(position):
            x, y = self.get_coordinates_on_board_by_pos(position)

            if self.if_pos_in_bound(x, y):
                self.set_piece(position, x, y, self._cur_player.get_color())
                if "captures" in kwargs.keys():
                    self.hide_captured(position, kwargs["captures"])
                    self._cur_player.add_captures(len(kwargs["captures"]))
                if "hints" in kwargs.keys():
                    self.print_hints(**kwargs)

                opponent = self._p2 if self._cur_player == self._p1 else self._p1
                strike = self._board.get_win_strike(position)
                if (len(strike) != 0 \
                        and (opponent.captures() + self._board.get_max_of_captures_by_pos(strike, self._cur_player.get_color()) < 10)) \
                    or self._cur_player.captures() >= 10:
                    self.print_winner(strike)
                else:
                    self._next()

    def _switch_player(self):
        if self._cur_player == self._p1:
            self._cur_player = self._p2
        else:
            self._cur_player = self._p1
        self._turn.set(self._cur_player.get_name())
        self._show_move_time()
        self._time_spend = time.perf_counter()
        self._info_frame.update_idletasks()

    def back(self):
        self.delete_hints()
        if self._winner is not None:
            self._show_move_time()
            self._delete_win_strike()
            self._switch_player()

        if self._moves.get() != 0:
            p = next(self._pieces[i] for i in range(len(self._pieces) - 1, -1, -1) if not self._pieces[i].is_captured())
            self.delete_piece(p)
            self._moves.set(self._moves.get() - 1)
            self._switch_player()

            pos = p.get_pos()
            captures = None
            if pos in self._captured.keys():
                self._cur_player.undo_captures(len(self._captured[pos]))
                self.show_captured(pos)
                captures = [c.get_pos() for c in self._captured[pos]]
            self._send_method("back", p.get_pos(), captures)

    def _next(self):
        self._moves.set(self._moves.get() + 1)
        self._switch_player()

    def if_pos_in_bound(self, x, y):
        return self._padding / 2 < x < self._board_width - self._padding / 2 and \
               self._padding / 2 < y < self._board_width - self._padding / 2

    def get_coordinates_on_board_by_pos(self, position):
        column = ord(position[0]) - ord('a')
        row = self._config.get_board_size() - int(position[1:])

        x = self._padding + self._cell_width * column
        y = self._padding + self._cell_width * row
        return x, y

    def _print_win_strike(self, strike):
        for position in strike:
            x, y = self.get_coordinates_on_board_by_pos(position)
            outline = "white" if self._cur_player.get_color() == "black" else "black"
            p = BoardGui.create_circle(self._board_canvas, x, y, self._piece_radius,
                                       fill=self._cur_player.get_color(),
                                       outline=outline, width=2)
            self._win_strike.append(Piece(p, position, self._cur_player.get_color()))

    def _delete_win_strike(self):
        self._board_canvas.bind('<Button-1>', self.make_turn_on_event)
        self._winner.destroy()
        self._winner = None
        self._winner_name.destroy()
        self._winner_name = None
        for piece in self._win_strike:
            self._board_canvas.delete(piece.get_piece())
        self._win_strike = []

    def set_pieces(self, positions, color, **kwargs):
        for pos in positions:
            x, y = self.get_coordinates_on_board_by_pos(pos)
            self.set_piece(pos, x, y, color, **kwargs)

    def delete_pieces(self, positions):
        for pos in positions:
            self.delete_piece(pos)

    def set_piece(self, position, x, y, color, **kwargs):
        if not self.piece_on_table(position):
            p = BoardGui.create_circle(self._board_canvas, x, y, self._piece_radius,
                                       fill=color, **kwargs)
            self._board_canvas.pack()
            self._pieces.append(Piece(p, position, color))
            self._board.set_piece_by_pos(position, color)
            return True
        return False

    def piece_on_table(self, position):
        for i in range(len(self._pieces) - 1, -1, -1):
            if position == self._pieces[i].get_pos():
                return not self._pieces[i].is_captured()
        return False

    def delete_piece(self, piece):
        self._board_canvas.delete(piece.get_piece())
        self._board.delete_piece_by_pos(piece.get_pos())
        self._pieces.remove(piece)

    def hide_captured(self, position, captured):
        self._captured[position] = []
        for pos in captured:
            piece = next(x for x in self._pieces if x == pos and not x.is_captured())
            self._captured[position].append(piece)
            self._board_canvas.itemconfigure(piece.get_piece(), state='hidden')
            self._board.delete_piece_by_pos(piece.get_pos())
            piece.catch()

    def show_captured(self, position):
        if position in self._captured.keys():
            for piece in self._captured[position]:
                self._board_canvas.itemconfigure(piece.get_piece(), state='normal')
                piece.uncatch()
                self._board.set_piece_by_pos(piece.get_pos(), piece.get_color())

    def _send_method(self, method_name, position, captures=None):
        if captures is None:
            captures = []
        arguments = {
            "position": position,
            "color": self._cur_player.get_color(),
            "captures": captures
        }
        self._send_func(method=method_name, arguments=arguments)

    def get_cur_player_name(self):
        return self._cur_player.get_name()

    def get_info_frame_rel_x(self):
        return self._info_frame.place_info()['relx']

    @staticmethod
    def create_circle(canvas, x, y, r, **kwargs):
        circle = canvas.create_oval(x - r, y - r, x + r, y + r, **kwargs)
        canvas.update_idletasks()
        return circle

    @staticmethod
    def round_cell(x, base):
        return base * round(x / base)

    def to_json(self):
        return json.dumps(self.__dict__())

    def __dict__(self):
        return {
            "size": self._config.get_board_size(),
            "turn": self._turn.get(),
            "moves": self._moves.get(),
            "current_player": self._cur_player.get_name(),
            "players": [self._p1.__dict__(), self._p2.__dict__()],
            "pieces": [p.__dict__() for p in self._pieces]
        }
