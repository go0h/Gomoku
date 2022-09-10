class BusyCell(Exception):
    def __init__(self, x: int, y: int):
        super().__init__(f'Cell ({x} : {y}) is busy')