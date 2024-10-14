import socket
import keyboard
import time
import json
import os

config_file = 'config.json'

if not os.path.exists(config_file):
    default_config = {
        'trigger_hotkey': '0x06',
        'always_enabled': False,
        'trigger_delay': 0.001,
        'base_delay': 0.01,
        'color_tolerance': 70,
        'zone': 1.5,
        'port': 65422
    }
    with open(config_file, 'w') as file:
        json.dump(default_config, file, indent=4)
    print(f'Keine config.json gefunden. Standardkonfiguration erstellt: {config_file}')
else:
    with open(config_file, 'r') as json_file:
        data = json.load(json_file)
    port = data.get('port', 65422)

def main():
    print('Loading...')
    print('Please open HW FREE.exe')
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind(('localhost', port))
    sock.listen(1)
    conn, addr = sock.accept()
    print(f'Connect From {addr} accepted.')
    with conn:
        print('Loaded.')
        while True:
            data = conn.recv(1)
            if data:
                key = data.decode()
                if len(key) == 1 and key.isalnum():
                    keyboard.send(key)
            else:
                break

if __name__ == '__main__':
    main()
