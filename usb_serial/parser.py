import glob
import numpy as np
import config
import os

# find files in the raw directory
files = glob.glob("raw/raw*")

os.system("rm output/*")


# number of arguments to expect per line
kargs = 14
ksensors = 12

# buffer for duplicate timestamps
buf_value = 100000

# take raw data files and convert to a dictionary of timestamps
# each timestamp has an array of packets.
def parse_data(files):
    cycles = []
    # the data dictionary used to store all relevant values
    data = {}
    
    # iterate through all generated raw files
    for file in files:

        f = open(file, 'r')
        raw = f.readlines()
        f.close()

        # initialize buffer to zero, this is used to increment the timestamps if
        # a reset occurs during the reading.
        buf = 0

        # traverse through the file and add good data to dictionary
        for line in raw:
            # check if timestamp reset, add a fixed value
            if str(line[0]) == "t":
                # increment by a large enough value, say 1000
                buf += buf_value
            # check if line begins with correct letter
            if line[0] != "I":
                continue

            # split line by whitespace
            line = line.split()

            # verify line is the correct length
            if len(line) != kargs:
                continue

            # create pseudo timestamp
            time = int(buf) + int(line[3])

            # populate packet with this line's info. this will be appended to the
            # data dictionary at the given timestamp.
            packet = {}
            packet["id"] = line[1]
            packet["led_state"] = line[5]
            packet["r"] = line[7]
            packet["g"] = line[9]
            packet["b"] = line[11]
            packet["c"] = line[13]

            # add packet to appropriate time in the data dictionary
            if time in data.keys():
                data[time].append(packet)
            else:
                data[time] = []
                data[time].append(packet)

    # trim data to only contain timestamps for which we have all data.
    # if that timestamp has exactly ksensors number of packets,
    # then we have received all the data for that timestamp.
    bad_times = []
    for time in data.keys():
        if len(data[time]) != ksensors:
            bad_times.append(time)

    for time in bad_times:
        del data[time]


    return data

# take sensor-wise state values and determine true state id
# from the configured lighting pattern
def determine_state(vec, mem):
    mat = config.CFG["state_matrix"]
    
    krow, kcol = mat.shape
    # array used to find rows which match the state
    match = np.array([0]*krow)
    
    i = 0
    for row in mat:
        if (row == vec).all():
            match[i] = 1
        i += 1

    match_indices = np.array(match.nonzero()).flatten()
    if len(match_indices) == 1:
        return match_indices[0]
    elif len(match_indices) == 0:
        print("======= ERROR: state not found")
        return -1


    inc = 0
    # continue looking until we have gone through all memory
    while (inc < len(mem)):
        mem_match_indices = [] # populate with current step match
        prev_mem_val = mem[-inc] # get mem row corresponding to step

        for match in match_indices:
            prev_mem_loc = match - inc
            row = mat[prev_mem_loc,:]
            if (row == prev_mem_val).all():
                # if this previous value is consistent
                # add to new match list
                mem_match_indices.append(prev_mem_loc)

        if len(mem_match_indices) == 1:
            return mem_match_indices[0]
        match_indices = mem_match_indices
        # go back another step
        inc += 1
    print("warning, skipping additional matches")
    if len(match_indices) > 0:
        return match_indices[0]
    else:
        print("Could not accurately find state id")
        return None
    

# send parsed to one master file
def save_to_file(data, filename):
    # iterate through all timestamps and generate strings to print
    inc = 0
    mem = []

    file_num = 0
    
    this_file = filename + "_" + str(file_num) + ".txt"
    print("created file: ", this_file)
    f = open(this_file, "w")
    
    for time in data.keys():
        
        this_time = data[time]
        output = {}
        # initialize state vector to populate with sensor-wise state info.
        # this will be compared against the expected pattern in the config
        # file to get the state that the system is in at this timestamp
        state_vec = np.array([-1]*ksensors)
        # get data from each sensor at the given timestamp.
        for i in range(0, ksensors):
            sensor = this_time[i]
            this_id = sensor["id"]

            # get specific sensor state and update state_vec
            state = sensor["led_state"]
            
            state_vec[int(this_id)] = int(state)

            # generate output string
            output[int(this_id)] = "\tID: " + this_id + "\tState: " + state + " R: " + sensor["r"] + " G: " + sensor["g"] + " B: " + sensor["b"] + " C: " + sensor["c"]+"\n"

        # get true state id from the configured lighting pattern
        state_id = determine_state(state_vec, mem)
        if state_id == 0:
            this_file = filename + "_" + str(file_num) + ".txt"
            print("created file: ", this_file)
            f.close()
            f = open(this_file, "w")
            file_num += 1
        mem.append(state_vec)
        if len(mem) > 2*ksensors:
            mem.pop(0)

        # write data
        # get rid of excess buffer created
        time = time % buf_value
        line = "Timestamp: " +  str(time) + " State ID: " + str(state_id)+"\n"
        f.write(line)
        for i in range(0,ksensors):
            f.write(output[i])
    f.close()

if __name__ == "__main__":
    # create dictionary of all timestamps
    my_data = parse_data(files)

    # format to strings and write to file
    save_to_file(my_data, "output/output")
