import socket
import os
import dotenv
import json
import time
import logging
import typing as t


PROJECT_DIR = os.path.dirname(os.path.dirname(os.getcwd()))
RESOURCES = os.path.join(PROJECT_DIR, "resources", "methods")

dotenv.load_dotenv()

logger = logging.getLogger(__name__)


class Client:
    def __init__(self,
                 host=os.environ['HOST'],
                 port=os.environ['PORT'],
                 client_encoding=os.environ['CLIENT_ENCODING'],
                 client_batch_size=os.environ['CLIENT_BATCH_SIZE'],
                 server_encoding=os.environ['CLIENT_ENCODING'],
                 server_batch_size=os.environ['CLIENT_BATCH_SIZE']
                 ):
        self._host: str = host
        self._port: int = int(port)
        self._client_encoding: str = client_encoding
        self._client_batch_size = int(client_batch_size)
        self._server_encoding: str = server_encoding
        self._server_batch_size = int(server_batch_size)
        self._connection = None

    @property
    def connection(self):
        if self._connection is not None:
            return self._connection
        self._connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        return self._connection

    def connect_to_server(self) -> None:
        self.connection.connect((socket.gethostbyname(self._host), self._port))

    def send_data(self, message: str) -> None:
        self.connection.send((message + "\n").encode(self._client_encoding))
        print(f'Client send:\n {json.loads(message)}')

    def receive_response(self) -> str:
        message = self.connection.recv(self._server_batch_size).decode(self._server_encoding)
        print(f'Client receive:\n {message}')
        return message

    def get_data(self) -> t.Generator:
        while self.connection:
            yield self.connection.recv(self._server_batch_size).decode(self._server_encoding)

    def close(self):
        self._connection.close()
        self._connection = None


if __name__ == '__main__':
    client = Client()
    client.connect_to_server()
    try:
        for file in sorted(os.listdir(f'{RESOURCES}')):
            with open(os.path.join(RESOURCES, file), 'r') as f:
                data = f.read()
                client.send_data(data)
                response = client.receive_response()
            time.sleep(3)
    except Exception as exc:
        print(exc)
    finally:
        client.connection.close()
