import os
import random
import typing as t
import dotenv
import socket
import json
import time

dotenv.load_dotenv()

MOVES = []

METHODS = ["back", "end_game", "make_turn", "print_hints", "start_game", "winner"]


def fill_moves(size):
    MOVES.clear()
    for r in range(size):
        for c in range(size):
            MOVES.append(chr(97 + c) + str(r + 1))


def get_random_move():
    pos = random.choice(MOVES)
    MOVES.remove(pos)
    return pos


def remove_move(move):
    MOVES.remove(move)


class Server:
    def __init__(self):
        self._socket = None
        self.connection = None
        self.address = None
        self.mode = None

    @property
    def socket(self):
        if self._socket is not None:
            return self._socket
        self._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self._socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self._socket.bind((socket.gethostbyname('localhost'), int(os.environ['PORT'])))
        return self._socket

    def accept_connection(self):
        self.socket.listen(0)
        self.connection, self.address = self.socket.accept()

    def get_data(self) -> t.Generator:
        while True:
            data = self.connection.recv(int(os.environ['SERVER_BATCH_SIZE'])).decode(os.environ['SERVER_ENCODING'])
            if not data:
                break
            data = json.loads(data)
            if data.get('method') == 'start':
                self.mode = data["arguments"]["mode"]
                # генерация тестовых данных
                fill_moves(int(data["arguments"]["board_size"]))
            elif data.get('method') == 'end_game':
                print(f"Game ends: {data.get('arguments')}")
                self.connection.send(json.dumps(data).encode(os.environ['SERVER_ENCODING']))
                break
            else:
                if data.get('method') in METHODS:
                    time.sleep(0.5)
                    # отправляем рандомный неповторяющийся ответ

                    if self.mode == "PvE":
                        remove_move(data["arguments"]["position"])
                        answer = {
                            "method": "make_turn",
                            "arguments": {
                                "position": get_random_move(),
                                "color": "white",
                                "hints": [random.choice(MOVES) for _ in range(3)]
                            }
                        }
                    else:
                        answer = {
                            "method": "print_hints",
                            "arguments": {
                                "hints": [random.choice(MOVES) for _ in range(3)]
                            }
                        }
                    arguments = json.dumps(answer)
                    self.connection.send(arguments.encode(os.environ['SERVER_ENCODING']))
                else:
                    print(f"Unresolved method {data.get('method')}")
                yield data

    def listen_connection(self):
        for message in self.get_data():
            if message:
                print(message)
                print()
        self.connection.close()


if __name__ == '__main__':
    while True:
        server = Server()
        server.accept_connection()
        server.listen_connection()
