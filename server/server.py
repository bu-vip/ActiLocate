import socket
import struct
import numpy as np

from config import CFG


#Data structure used to store readings from a single state
DATA = {
    "timestamp": -1, 
    'data': [{"timestamp":-1, "state":-1, "R":-1, "G":-1, "B":-1, "C":-1} for i in range(len(CFG['state_matrix'][0]))]
}


#Start connection
sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind((CFG['host'], CFG['port']))


#Run server
while True:
    
    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    
    try:
        #Check packet is proper length
        assert len(data)==14

        #Get id and timestamp of sensor sending data
        id = struct.unpack('B', data[0])[0]
    	timestamp = struct.unpack('<L', data[1:5])[0]
        
        if timestamp >= DATA['timestamp'] + 1:
            if DATA['timestamp'] != -1:
                #Check this is not simply the first packet read
                #identify state ID
                states = np.array([DATA['data'][i]['state'] for i in range(len(DATA['data']))])
                for state_id in range(len(CFG['state_matrix'])):
                    if np.array_equal(states, CFG['state_matrix'][state_id]):
                        break
                    state_id = -1
               
                #Output file format will be <cycle#><out_file> where cycle # is the number of iterations through the matrix that have occured
                #For example, if this is the 0th itertaion and the out_file is "out.txt" output file will be "0out.txt"
                with open(str(DATA['timestamp']/(len(DATA['data']) + 1)) + CFG['out_file'], 'a+') as outfile:
                    
                    outfile.write("State ID: %d Timestamp %d\r\n" % (state_id, DATA['timestamp']))
                    for i in range(len(DATA['data'])):
                        outfile.write(
                            "\tID: %d State: %d R: %d G: %d B: %d C: %d\r\n" % (
                            i,
                            DATA['data'][i]['state'],
                            DATA['data'][i]['R'],
                            DATA['data'][i]['G'],
                            DATA['data'][i]['B'],
                            DATA['data'][i]['C'])
                        )
                    outfile.write("\r\n")
            #New state
            DATA['timestamp'] = timestamp

        #Store data
        DATA['data'][id]['timestamp'] = timestamp
        DATA['data'][id]['state'] = struct.unpack('B', data[5])[0]
      	DATA['data'][id]['R'] = struct.unpack('<H', data[6:8])[0]
        DATA['data'][id]['G'] = struct.unpack('<H', data[8:10])[0]
      	DATA['data'][id]['B'] = struct.unpack('<H', data[10:12])[0]
      	DATA['data'][id]['C'] = struct.unpack('<H', data[12:14])[0]
        
        print (
            "ID: %d |  STATE: %d | TIME: %d | RED: %d | GREEN %d | BLUE: %d | CLEAR: %d" % (
                id,
                DATA['data'][id]['state'],
                timestamp,
                DATA['data'][id]['R'],
                DATA['data'][id]['G'],
                DATA['data'][id]['B'],
                DATA['data'][id]['C']
            )
        )

    except AssertionError:
	    print "error! bad packet with length, ", len(data)	
