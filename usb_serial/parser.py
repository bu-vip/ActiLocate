import glob
import config

# find files beginning with ACM in this directory
files = glob.glob("raw/raw*")


# number of arguments to expect per line
kargs = 14

data = {}

for file in files:
    print(file)
    f = open(file, 'r')
    raw = f.readlines()

    #init id
    id = -1

    buf = 0
    for line in raw:
        # check if timestamp reset, add a fixed value
        if str(line[0]) == "t":
            buf += 1000
        # check if line begins with correct letter
        if line[0] != "I":
            continue
        # split line by whitespace
        line = line.split()

        if len(line) != kargs:
            print("Warning: skipping line... invalid number of arguments")
            continue

        # assign id
        time = int(buf) + int(line[3])
        # populate object with all pertinant info
        packet = {}
        packet["id"] = line[1]
        packet["led_state"] = line[5]
        packet["r"] = line[7]
        packet["g"] = line[9]
        packet["b"] = line[11]
        packet["c"] = line[13]
        if time in data.keys():
            data[time].append(packet)
        else:
            data[time] = []
            data[time].append(packet)

print("NUM KEYS: ", len(data.keys()))

num_sensors = 12
prev_id = -1
state_id = -1
for time in data.keys():
    this_time = data[time]
    output = {}
    if len(this_time) == num_sensors:
        for i in range(0,num_sensors):
            sensor = this_time[i]
            this_id = sensor["id"]
            state = sensor["led_state"]
            if int(state) == 1:
                state_id = this_id
            output[int(this_id)] = "\tID: " + this_id + "\tState: " + state + " R: " + sensor["r"] + " G: " + sensor["g"] + " B: " + sensor["b"] + " C: " + sensor["c"]

        if state_id == prev_id:
            state_id=-1
        if state_id != prev_id:
            prev_id = state_id
        print("Timestamp: ", time, " State ID: ", state_id)
        for i in range(0,num_sensors):
            print(output[i])

