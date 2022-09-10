import argparse
import dotenv
import os

from gui.constants import WIN_WIDTH, WIN_HEIGHT
from gui.client import Client
from gui.gomoku_gui import GomokuGui

if __name__ == '__main__':
    dotenv.load_dotenv()
    parser = argparse.ArgumentParser(description="Gomoku game")
    parser.add_argument("--host", nargs="?", type=str, default=os.environ['HOST'], help="Server address")
    parser.add_argument("--port", nargs="?", type=str, default=os.environ['PORT'], help="Server port")
    args = parser.parse_args()

    client = Client(host=args.host, port=args.port)
    gui = GomokuGui(WIN_WIDTH, WIN_HEIGHT, client=client)
    gui.start()
