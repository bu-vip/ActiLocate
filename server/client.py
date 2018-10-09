import socket
import random
import string
import time
import struct

from config import CFG

SERVER_IP = "127.0.0.1"
POLLING_PERIOD = 83 #milliseconds

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP


# ID(1), STATE(1), TIME(8), DATA(8)
state = 0
timestamp = long(0)
while True:

    time.sleep(float(POLLING_PERIOD)/1000)


    for id in range(len(CFG['state_matrix'][state])):
        
        r = random.randint(0,65535)
        g = random.randint(0,65535)
        b = random.randint(0,65535)
        c = random.randint(0,65535)
        message = struct.pack('B', id) + struct.pack('<L', timestamp) + struct.pack('B', CFG['state_matrix'][state][id]) + struct.pack('<H', r) + struct.pack('<H', g) + struct.pack('<H', b) + struct.pack('<H', c)

        print "message:", message.encode('string_escape')
        sock.sendto(message, (SERVER_IP, CFG['port']))
    timestamp = timestamp + 1
    if state == len(CFG['state_matrix']) - 1:
        state = 0
    else:
        state = state + 1
    