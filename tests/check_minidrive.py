"""Loopback smoke check; all server data is created in a temporary directory."""
import json
from pathlib import Path
import socket
import struct
import subprocess
import tempfile
import time

ROOT = Path(__file__).resolve().parents[1]


def receive_exact(sock, count):
    result = b''
    while len(result) < count:
        chunk = sock.recv(count - len(result))
        if not chunk:
            raise RuntimeError('Connection closed before a complete response')
        result += chunk
    return result


def exchange(sock, value):
    payload = json.dumps(value).encode()
    sock.sendall(struct.pack('!I', len(payload)) + payload)
    length = struct.unpack('!I', receive_exact(sock, 4))[0]
    if length > 1024 * 1024:
        raise RuntimeError('Unexpected response size')
    return json.loads(receive_exact(sock, length))


def main():
    with tempfile.TemporaryDirectory() as directory:
        root = Path(directory)
        (root / 'public').mkdir()
        (root / 'public' / 'demo.txt').write_text('Synthetic smoke-test data\n')
        with socket.socket() as reservation:
            reservation.bind(('127.0.0.1', 0))
            port = reservation.getsockname()[1]
        server = subprocess.Popen(
            [str(ROOT / 'minidrive/build/server/server'), '--port', str(port), '--root', str(root)],
            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        try:
            deadline = time.monotonic() + 5
            while True:
                try:
                    connection = socket.create_connection(('127.0.0.1', port), timeout=1)
                    break
                except OSError:
                    if server.poll() is not None or time.monotonic() >= deadline:
                        raise RuntimeError('MiniDrive server did not start')
                    time.sleep(0.05)
            with connection:
                connection.settimeout(3)
                reply = exchange(connection, {'type': 'public'})
                assert reply['status'] == 'OK', reply
                reply = exchange(connection, {'cmd': 'LIST', 'args': {}})
                assert reply['status'] == 'OK', reply
                assert any(v['name'] == 'demo.txt' for v in reply['data']['entries']), reply
                reply = exchange(connection, {'cmd': 'MKDIR', 'args': {'path': 'example'}})
                assert reply['status'] == 'OK', reply
                assert (root / 'public' / 'example').is_dir()
            print('MiniDrive: public handshake, LIST and MKDIR passed')
        finally:
            server.terminate()
            try:
                server.wait(timeout=3)
            except subprocess.TimeoutExpired:
                server.kill()
                server.wait(timeout=3)


if __name__ == '__main__':
    main()
