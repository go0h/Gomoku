
import json

BOARD_SIZE = {"19 x 19": 19, "15 x 15": 15}


class GameConfig:

    def __init__(self, mode, difficult, board_size, player_color):
        self.mode = mode
        self.difficult = difficult
        self.board_size = BOARD_SIZE[board_size]
        self.player_color = player_color

    @staticmethod
    def create(mode, difficult, board_size, first_move):
        return GameConfig(mode, difficult, board_size, first_move)

    def get_board_size(self):
        return self.board_size

    def get_mode(self):
        return self.mode

    def get_difficult(self):
        return self.difficult

    def get_first_move(self):
        return self.player_color

    def __str__(self):
        return f"Mode: {str(self.mode)}\nDifficult: {str(self.difficult)}\n" + \
               f"Board size: {str(self.board_size)}\nFirst move: {str(self.player_color)}\n"

    def to_json(self):
        return json.dumps(self.__dict__)