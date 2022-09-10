class ForbiddenTurn(Exception):
    def __init__(self, coord: str):
        super().__init__(f'Turn in {coord} is forbiden')