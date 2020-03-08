import socket
import pickle


class Network:
    """Class which ease the communication with the server"""

    def __init__(self):
        self.client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # Communication using Ipv4 and streams
        self.server = "192.168.1.181"  # Sever ip the only thing which have to be changed to properly connect to the server
        self.port = 10000  # Port number used to communication (High number ~ unused by other programs)
        self.addr = (self.server, self.port)

    def connect(self, name):
        """Establish connection with the server and try to send player name"""

        try:
            self.client.connect(self.addr)  # Trying to establish communication with the server - sending connection request
            self.client.send(pickle.dumps(name))  # Trying to send our player name
            return pickle.loads(self.client.recv(10000))
        except Exception as e:
                print(e)

    def diconnect(self):
        """Disconnect with the server (In more advanced program it is important to close our connections which close the file descriptors)"""

        self.client.send(pickle.dumps(123))
        self.client.close()

    def send(self, data):
        """Convert the argument(coded move) using pickle and send it to our server
        after that try to receive data with updated players and food cells and return it"""

        try:
            self.client.send(pickle.dumps(data))
            reply = self.client.recv(10000)
            try:
                reply = pickle.loads(reply)
            except Exception as e:
                print(e)

            return reply
        except socket.error as e:
            print(e)
