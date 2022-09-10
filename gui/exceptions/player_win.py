import typing as t


class BlackPlayerWinException(Exception):
    def __init__(self, coordinates: t.List[t.Tuple[int, int]]):
        super().__init__('Black Player Win')
        self.win_coordinates = coordinates


class WhitePlayerWinException(Exception):
    def __init__(self, coordinates: t.List[t.Tuple[int, int]]):
        super().__init__('White Player Win')
        self.win_coordinates = coordinates
