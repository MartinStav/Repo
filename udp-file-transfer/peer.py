import socket
import threading
import struct
import random
import time
import os

TYPE_BITS = 3
FLAG_BITS = 1
HEADER_FORMAT = f'>H H I'
HEARTBEAT_INTERVAL = 5
ACK_TIMEOUT = 1

class Peer:
    def __init__(self, sourceIP, sourcePort, destinationIP, destinationPort, seqNum) -> None:
        self.sourceIP = sourceIP
        self.sourcePort = sourcePort
        self.seqNum = seqNum
        self.peerSeqNum = 0
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind((sourceIP, sourcePort))
        self.peer_address = (destinationIP, destinationPort)
        self.isConnected = False
        self.last_heartbeat = time.time()
        self.fragment_buffer = {}
        self.isDamaged = False
        self.save_directory = os.getcwd()
        self.heartbeat_count = 1
        self.message_time = time.time()

    def calculate_crc(self, data, poly=0x8005, init_crc=0xFFFF):
        crc = init_crc
        for byte in data:
            crc ^= byte << 8
            for _ in range(8):
                if crc & 0x8000:
                    crc = (crc << 1) ^ poly
                else:
                    crc <<= 1
                crc &= 0xFFFF
        return crc

    def create_header(self, message_type, fragment_flag, sequence_number, checksum, fragment_number):
        header = struct.pack(HEADER_FORMAT,
                            (message_type << (16 - TYPE_BITS)) |
                            (fragment_flag << (16 - TYPE_BITS - FLAG_BITS)) |
                            sequence_number,
                            checksum,
                            fragment_number)
        return header

    def read_header(self, data):
        pack, checksum, fragment_number = struct.unpack(HEADER_FORMAT, data)

        header = (
            (pack >> (16 - TYPE_BITS)) & 0x7,
            (pack >> (16 - TYPE_BITS - FLAG_BITS)) & 0x01,
            pack & 0xFFF,
            checksum,
            fragment_number)
        return header

    def increase_sequence_number(self):
        self.seqNum += 1
        if self.seqNum == 2**12:
            self.seqNum = 0

    def send_message(self, message, max_payload_size):
        print(f"[INFO] Sending message: {message} (size: {len(message)} B)")
        self.increase_sequence_number()
        message_type = 5

        message = message.encode('utf-8')
        message_length = len(message)
        total_fragments = (message_length // max_payload_size) + (1 if message_length % max_payload_size > 0 else 0)
        if total_fragments > 2**32:
            print("[ERROR] Message too long to send.")
            return

        for fragment_number in range(total_fragments):
            start_index = fragment_number * max_payload_size
            end_index = start_index + max_payload_size
            fragment = message[start_index:end_index]

            fragment_flag = 1 if fragment_number == total_fragments - 1 else 0
            checksum = self.calculate_crc(fragment)

            header = self.create_header(message_type, fragment_flag, self.seqNum, checksum, fragment_number)
            self.fragment_buffer[(self.seqNum, fragment_number)] = (header + fragment, time.time())

            if self.isDamaged and fragment_number % 5 == 1:
                checksum = 0
                header = self.create_header(message_type, fragment_flag, self.seqNum, checksum, fragment_number)

            if not (self.isDamaged and fragment_number % 5 == 0):
                self.sock.sendto(header + fragment, self.peer_address)
            print(f"[DEBUG] Sent fragment {self.seqNum}: {fragment_number}/{total_fragments - 1}: {fragment.decode('utf-8', errors='replace')} (size: {len(fragment)} B)")

    def send_file(self, filepath, max_payload_size):
        self.increase_sequence_number()
        message_type = 6
        filename = os.path.basename(filepath).encode('utf-8')

        try:
            with open(filepath, 'rb') as file:
                file_content = file.read()
        except FileNotFoundError:
            print("[ERROR] File not found.")
            return

        print(f"[INFO] Sending file: {filepath} (size: {os.path.getsize(filepath)} B)")
        data = filename + b'\0' + file_content
        total_fragments = (len(data) // max_payload_size) + (1 if len(data) % max_payload_size > 0 else 0)
        if total_fragments > 2**32:
            print("[ERROR] File too large to send.")
            return

        for fragment_number in range(total_fragments):
            start_index = fragment_number * max_payload_size
            end_index = start_index + max_payload_size
            fragment = data[start_index:end_index]

            fragment_flag = 1 if fragment_number == total_fragments - 1 else 0
            checksum = self.calculate_crc(fragment)

            header = self.create_header(message_type, fragment_flag, self.seqNum, checksum, fragment_number)
            self.fragment_buffer[(self.seqNum, fragment_number)] = (header + fragment, time.time())

            if self.isDamaged and fragment_number % 5 == 1:
                checksum = 0
                header = self.create_header(message_type, fragment_flag, self.seqNum, checksum, fragment_number)

            if not (self.isDamaged and fragment_number % 5 == 0):
                self.sock.sendto(header + fragment, self.peer_address)
            print(f"[DEBUG] Sent file fragment {self.seqNum}: {fragment_number + 1}/{total_fragments}")

    def request_wrong_fragment(self, fragment_number, sequence_number):
        message_type = 4

        header = self.create_header(message_type, 0, sequence_number, 0, fragment_number)
        self.sock.sendto(header, self.peer_address)
        #print(f"[DEBUG] Requested fragment {fragment_number} for sequence {sequence_number}")

    def confirm_fragment(self, fragment_number, sequence_number):
        message_type = 3

        header = self.create_header(message_type, 0, sequence_number, 0, fragment_number)
        self.sock.sendto(header, self.peer_address)
        #print(f"[DEBUG] Confirmed fragment {fragment_number} for sequence {sequence_number}")

    def timeout_message(self):
        while self.isConnected:
            time.sleep(ACK_TIMEOUT)
            current_time = time.time()
            for (seq_num, fragment_num), (packet, timestamp) in list(self.fragment_buffer.items()):
                if current_time - timestamp >= ACK_TIMEOUT:
                    print(f"[WARNING] Resending fragment {seq_num}: {fragment_num} due to timeout.")
                    self.sock.sendto(packet, self.peer_address)
                    self.fragment_buffer[(seq_num, fragment_num)] = (packet, current_time)

    def listen_for_messages(self):
        received_fragments = {}
        total_fragments = None
        error_fragments = set()

        while self.isConnected:
            data, _ = self.sock.recvfrom(1450)
            header_size = struct.calcsize(HEADER_FORMAT)
            header_data = data[:header_size]
            payload_data = data[header_size:]

            headerFields = self.read_header(header_data)

            message_type, fragment_flag, sequence_number, checksum, fragment_number = headerFields

            self.last_heartbeat = time.time()
            if message_type == 7:
                self.heartbeat_count = 1
                #print("[DEBUG] Received heartbeat")
                continue

            elif message_type == 0:
                self.isConnected = False
                print("[INFO] Connection closed")
                break

            elif message_type == 4:
                if (sequence_number, fragment_number) in self.fragment_buffer:
                    packet, _ = self.fragment_buffer[(sequence_number, fragment_number)]
                    self.sock.sendto(packet, self.peer_address)
                    print(f"[INFO] Resent fragment {sequence_number}: {fragment_number}")
                continue

            elif message_type == 3:
                if (sequence_number, fragment_number) in self.fragment_buffer:
                    del self.fragment_buffer[(sequence_number, fragment_number)]
                    #print(f"[INFO] Received ACK for fragment {fragment_number}.")
                continue

            if fragment_number == 0:
                self.message_time = time.time()
            crc = self.calculate_crc(payload_data)
            if crc != checksum:
                print(f"[ERROR] Checksum error on fragment {fragment_number}")
                error_fragments.add((sequence_number, fragment_number))
                self.request_wrong_fragment(fragment_number, sequence_number)
                continue

            received_fragments[fragment_number] = payload_data
            if fragment_flag == 1:
                total_fragments = fragment_number + 1
                for i in range(total_fragments):
                    if i not in received_fragments:
                        print(f"[WARNING] Missing fragment {i}")
                        error_fragments.add((sequence_number, fragment_number))
                        self.request_wrong_fragment(i, sequence_number)
                        continue
            print(f"[DEBUG] Received fragment {sequence_number}: {fragment_number} (size: {len(payload_data)} B)")
            self.confirm_fragment(fragment_number, sequence_number)
            if total_fragments and len(received_fragments) == total_fragments:
                full_data = b''.join(received_fragments[i] for i in range(total_fragments))
                if message_type == 6:
                    filename, file_content = full_data.split(b'\0', 1)
                    with open(os.path.join(self.save_directory, filename.decode('utf-8')), 'wb') as file:
                        file.write(file_content)
                    print(f"[SUCCESS] File '{filename.decode('utf-8')}' received successfully.")
                    print(f"[INFO] Size: {len(file_content)} B")
                    print(f"[INFO] File saved to {self.save_directory}")
                    print(f"[INFO] Time taken: {time.time() - self.message_time:.2f} s")
                elif message_type == 5:
                    print("[SUCCESS] Message received:", full_data.decode('utf-8'))
                    print(f"[INFO] Size: {len(full_data)} B")
                    print(f"[INFO] Time taken: {time.time() - self.message_time:.2f} s")

                print(f"[IMPLEMENTATION] Total fragments: {total_fragments}")
                print(f"[IMPLEMENTATION] Correct fragments: {total_fragments - len(error_fragments)}")
                print(f"[IMPLEMENTATION] Error fragments: {len(error_fragments)}")

                received_fragments.clear()
                total_fragments = None
                error_fragments.clear()

    def handshake(self):
        message_type = 1
        fragment_flag = 0
        fragment_number = 0
        checksum = 0

        header = self.create_header(message_type, fragment_flag, self.seqNum, checksum, fragment_number)
        self.sock.sendto(header, self.peer_address)
        print("[DEBUG] SYN sent")

        while not self.isConnected:
            data, _ = self.sock.recvfrom(1450)
            headerFields = self.read_header(data)
            messageType = headerFields[0]

            if messageType == 1:
                self.peerSeqNum = headerFields[2]
                message_type = 2
                header = self.create_header(message_type, fragment_flag, self.seqNum, checksum, fragment_number)
                self.sock.sendto(header, self.peer_address)
                print("[DEBUG] SYN-ACK sent")
                continue

            if messageType == 2:
                self.increase_sequence_number()
                self.peerSeqNum = headerFields[2]
                message_type = 3
                header = self.create_header(message_type, fragment_flag, self.seqNum, checksum, fragment_number)
                self.sock.sendto(header, self.peer_address)
                self.isConnected = True
                print("[DEBUG] ACK sent")
                break

            if messageType == 3:
                self.increase_sequence_number()
                self.isConnected = True
                break

    def send_heartbeat(self):
        while self.isConnected:
            time.sleep(HEARTBEAT_INTERVAL)

            if time.time() - self.last_heartbeat > HEARTBEAT_INTERVAL * 3:
                print("[ERROR] Heartbeat timeout, connection lost.")
                self.isConnected = False
                break
            else:
                message_type = 7
                header = self.create_header(message_type, 0, self.seqNum, 0, 0)
                if not self.sock:
                    break
                self.sock.sendto(header, self.peer_address)
                print(f"[DEBUG] Sent heartbeat ({self.heartbeat_count} / 3)")
                self.heartbeat_count += 1

    def close_connection(self):
        print("[INFO] Closing connection")
        header = self.create_header(0, 0, self.seqNum, 0, 0)
        self.sock.sendto(header, self.peer_address)
        self.isConnected = False

if __name__ == "__main__":
    sourcePort = int(input("[INPUT] Source Port: "))

    destIP = input("[INPUT] Destination IP (default localhost): ")
    if destIP == '':
        destIP = "localhost"
    destPort = int(input("[INPUT] Destination Port: "))

    peer = Peer('0.0.0.0', sourcePort, destIP, destPort, random.randint(0, 2**12 - 1))

    print("[INFO] Connection initiated")
    print("[INFO] Source port:", destPort)
    print("[INFO] Destination IP:", destIP)
    peer.handshake()
    threading.Thread(target=peer.listen_for_messages, daemon=True).start()
    threading.Thread(target=peer.send_heartbeat, daemon=True).start()
    threading.Thread(target=peer.timeout_message, daemon=True).start()

    max_payload_size = 1450
    print("[INFO] Connection established. Type /help for help.")
    while peer.isConnected:
        message = input("")
        if message == "/help":
            print(f"\n[INFO] HELP \n<message>\n/file <filepath>\n/save <filepath>\n/size <1 - 1450>\n/damaged <0 - False | 1 - True>\n/exit\n")
        elif message.startswith("/file "):
            filepath = message.split("/file ", 1)[1]
            peer.send_file(filepath, max_payload_size)
        elif message.startswith("/save "):
            filepath = message.split("/save ", 1)[1]
            if os.path.isdir(filepath):
                peer.save_directory = filepath
                print(f"[INFO] Files will be saved to {filepath}")
            else:
                peer.save_directory = os.getcwd()
                print(f"[INFO] Files will be saved to {peer.save_directory}")
        elif message.startswith("/size "):
            try:
                size_value = int(message.split()[1])
                if size_value < 1 or size_value > 1450:
                    print(f"[ERROR] Please provide a number 0 < size < 1450.")
                else:
                    max_payload_size = size_value
            except (IndexError, ValueError):
                print("[ERROR] Please provide a valid number after /size.")
        elif message.startswith("/damaged "):
            try:
                damaged = int(message.split()[1])
                if damaged == 0:
                    peer.isDamaged = False
                    print("[INFO] Damaged mode set to False.")
                else:
                    peer.isDamaged = True
                    print("[INFO] Damaged mode set to True.")
            except (IndexError, ValueError):
                print("[ERROR] Invalid input for damaged mode.")
        elif message == "/exit":
            peer.close_connection()
        else:
            peer.send_message(message, max_payload_size)
