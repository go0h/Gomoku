from enum import Enum


class Color(Enum):
    WHITE = 1
    BLACK = 2
    EMPTY = 0

    def __str__(self):
        if self == Color.WHITE:
            return '0'
        elif self == Color.BLACK:
            return 'x'
        else:
            return '_'

    def __repr__(self):
        return self.__str__()
