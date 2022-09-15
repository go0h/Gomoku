import json
from operator import le
import re
import tkinter as ttk
import webbrowser as wb

from threading import Thread
from gui import BoardGui, board
from gui import GameConfig

from constants import (
    WIN_HEIGHT, WIN_WIDTH,
    BACKGROUND_COLOR,
    FONT, COPYRIGHT_FONT_SIZE, LABEL_FONT_SIZE,
    LABEL_FONT, BUTTON_FONT, BUTTON_COLOR, BOARD_COLOR,
    OPTION_MENU_WIDTH, OPTION_MENU_HEIGHT
)


class GomokuGui:

    def __init__(self, width=WIN_WIDTH, height=WIN_HEIGHT, client=None):
        self._root = ttk.Tk()
        self._config = None
        self._board = None
        self._info_panel = None
        self._client = client
        self._listener = None

        self._root.title('Gomoku')
        self._root.config(bg=BACKGROUND_COLOR)

        x = int((self._root.winfo_screenwidth() / 2) - (width / 2))
        y = int((self._root.winfo_screenheight() / 2) - (height / 2))

        self._root.geometry(f"{width}x{height}+{x}+{y}")
        self._root.update()

    def start(self):
        try:
            self._client.connect_to_server()
            self._root.protocol("WM_DELETE_WINDOW", self._end_game)
            self._listener = Thread(target=self._listen_server)
            self._listener.start()
        except ConnectionRefusedError:
            self._client = None
            print("Can't connect to server. Game available only PvP mode")

        self.print_config()
        self._root.mainloop()

    def print_config(self):
        """Initialize Config menu and create game"""
        frame = ttk.Frame(self._root,
                          width=self._root.winfo_width(),
                          height=self._root.winfo_height(),
                          bg=BACKGROUND_COLOR)
        frame.pack()
        frame.place(relx=.5, rely=.5, anchor="center")

        copy_right = ttk.Label(frame,
                               text='© 21 School, 2022',
                               font=(FONT, COPYRIGHT_FONT_SIZE),
                               bg=BACKGROUND_COLOR)
        copy_right.place(relx=.5, rely=.97, anchor="center")

        welcome = ttk.Label(frame,
                            text='Welcome to Gomoku',
                            font=(FONT, LABEL_FONT_SIZE, "bold"),
                            bg=BACKGROUND_COLOR)
        welcome.place(relx=.5, rely=.1, anchor="center")

        game_rules = ttk.Label(frame,
                               text='Game rules',
                               font=(FONT, LABEL_FONT_SIZE, "underline", "bold"),
                               cursor="hand2",
                               bg=BACKGROUND_COLOR)
        game_rules.bind("<Button-1>", lambda e: wb.open_new("https://cdn.intra.42.fr/pdf/pdf/42138/en.subject.pdf"))
        game_rules.place(relx=.5, rely=.8, anchor="center")

        ch_mode = ttk.Label(frame,
                            text='Game mode:',
                            font=LABEL_FONT,
                            bg=BACKGROUND_COLOR)
        ch_mode.place(relx=.5, rely=.3, anchor="center")

        b1 = ttk.Button(frame,
                        text="Player VS AI",
                        font=BUTTON_FONT,
                        width=30, height=1,
                        bg=BUTTON_COLOR,
                        bd=4,
                        cursor="hand2")
        b1.place(relx=.5, rely=.4, anchor="center")

        b2 = ttk.Button(frame,
                        text="Player VS Player",
                        font=BUTTON_FONT,
                        width=30, height=1,
                        bg=BUTTON_COLOR,
                        bd=4,
                        cursor="hand2")
        b2.place(relx=.5, rely=.5, anchor="center")

        diff = ttk.Label(frame, text='Difficult', font=BUTTON_FONT, bg=BACKGROUND_COLOR)
        diff.place(relx=.35, rely=.59, anchor="center")

        hard = ttk.StringVar()
        hard.set("EASY")
        r1 = ttk.OptionMenu(frame, hard, "EASY", "MEDIUM", "HARD")
        r1.configure(font=BUTTON_FONT,
                     width=OPTION_MENU_WIDTH, height=OPTION_MENU_HEIGHT,
                     bg=BUTTON_COLOR, bd=4, cursor="hand2")
        drop = self._root.nametowidget(r1.menuname)
        drop.config(font=BUTTON_FONT, bg=BOARD_COLOR)
        r1.place(relx=.35, rely=.65, anchor="center")

        b_size = ttk.Label(frame, text='Board size', font=BUTTON_FONT, bg=BACKGROUND_COLOR)
        b_size.place(relx=.5, rely=.59, anchor="center")

        board_size = ttk.StringVar()
        board_size.set("19 x 19")
        r2 = ttk.OptionMenu(frame, board_size, "15 x 15", "19 x 19")
        r2.configure(font=BUTTON_FONT,
                     width=OPTION_MENU_WIDTH, height=OPTION_MENU_HEIGHT,
                     bg=BUTTON_COLOR, bd=4, cursor="hand2")
        drop = self._root.nametowidget(r2.menuname)
        drop.config(font=BUTTON_FONT, bg=BOARD_COLOR)
        r2.place(relx=.5, rely=.65, anchor="center")

        color_l = ttk.Label(frame, text='Color', font=BUTTON_FONT, bg=BACKGROUND_COLOR)
        color_l.place(relx=.65, rely=.59, anchor="center")

        color = ttk.StringVar()
        color.set("BLACK")
        r3 = ttk.OptionMenu(frame, color, "BLACK", "WHITE")
        r3.configure(font=BUTTON_FONT,
                     width=OPTION_MENU_WIDTH, height=OPTION_MENU_HEIGHT,
                     bg=BUTTON_COLOR, bd=4, cursor="hand2")
        drop = self._root.nametowidget(r3.menuname)
        drop.config(font=BUTTON_FONT, bg=BOARD_COLOR)
        r3.place(relx=.65, rely=.65, anchor="center")

        b1.configure(command=lambda: self.start_game(
            GameConfig.create("PvE", hard.get(), board_size.get(), color.get())))
        b2.configure(command=lambda: self.start_game(
            GameConfig.create("PvP", hard.get(), board_size.get(), color.get())))

        if self._client is None:
            b1["state"] = "disabled"
            r1["state"] = "disabled"
            r3["state"] = "disabled"

    def start_game(self, config):
        self._config = config
        for widget in self._root.winfo_children():
            widget.destroy()

        self._board = BoardGui(self._root, config, self._send_message_from_board())
        self._board.print_board()
        self.send_message(method="start_game", arguments=self._config.__dict__)

        back = ttk.Button(self._root, text="Back",
                          font=BUTTON_FONT,
                          width=20, height=1,
                          bg=BUTTON_COLOR, bd=4,
                          cursor="hand2")
        back.configure(command=self._board.back)
        back.place(relx=self._board.get_info_frame_rel_x(), rely=.8, anchor="center")

        back = ttk.Button(self._root, text="Restart",
                          font=BUTTON_FONT,
                          width=20, height=1,
                          bg=BUTTON_COLOR, bd=4,
                          cursor="hand2")
        back.configure(command=self.restart_game)
        back.place(relx=self._board.get_info_frame_rel_x(), rely=.9, anchor="center")

    def _end_game(self):
        self.send_message("end_game", {})
        self._listener.join()
        self._root.destroy()

    def restart_game(self):
        for widget in self._root.winfo_children():
            widget.destroy()
        self.print_config()

    def _listen_server(self):
        for message in self._client.get_data():

            if len(message) == 0:
                print(f'Server close connection')
                self._client.close()
                self._client = None
                return

            try:
                data = json.loads(message)
            except Exception as e:
                print(f'Client receive bad format data: {data}')
                continue

            print(f'Client receive:\n {data}')
            method_name = data.get("method")
            arguments = data.get("arguments")
            if method_name == "end_game":
                self._client.connection.close()
                break
            elif method_name in ["winner", "back", "start_game"]:
                print(f"Method '{method_name}' - OK")
            elif method_name in dir(self._board):

                # Test backend for forbidden moves
                if method_name == "make_turn":
                    pos = data["arguments"]["position"]
                    color = data["arguments"]["color"]
                    if self._board._board.is_forbidden_turn_pos(pos, color):
                        raise Exception(f"Bad move with position={pos} and color={color}")

                method = self._board.__getattribute__(method_name)
                method(**arguments)
            else:
                print(f"Unresolved method '{method_name}' with arguments: {arguments}")

    def send_message(self, method, arguments: dict):
        """Send player action to server """
        message = create_message(method, arguments)
        if self._client is not None:
            self._client.send_data(message)
        else:
            print(message)

    def _send_message_from_board(self):
        """Send player action to server"""
        def send(method, arguments: dict):
            message = create_message(method, arguments)
            if self._client is not None:
                self._client.send_data(message)
            else:
                print(message)
        return send


def create_message(method, arguments):
    res = {
        "method": method,
        "arguments": arguments
    }
    return json.dumps(res)
