# parameter: file_name, num_of_user, length_per_segment 
#e.g. python tmm_process.py 2u_10M.txt 2 1


import sys
import numpy as np
input_file = sys.argv[1] # input file name
num_user = sys.argv[2] # num of user
seg_len = sys.argv[3] # length per segment
in_file = open(input_file,'r')


tmp = input_file.split('.')
#output = tmp[0]+"_trans.csv"
#output_band = tmp[0]+"_band.csv"
#output_time = tmp[0]+"_time.csv"
output_new2 = tmp[0]+"_new2.csv"
#output_get = tmp[0]+"_get.csv"
#output_stall = tmp[0]+"_stall.csv"
# first value of each line 
'''
1. dash-server (the time that server send the packet) each packet 1356bytes
2. dash-client (the time that each user receive the packet)
3. segment-expected playout time (the first segment's playout time)
4. stall time (the time difference bewteen stall and resume)
5. segment packet

6. Client request time
'''

dash_server = list()
dash_client = list()
exp_time = list()
stall_time = list()
segment_packet = list()
client_request = list()


for line in in_file:
	line = line.strip()
	first = line.split(',')
	if first[0]==str(1):
		dash_server.append(first[1:])
	elif first[0]==str(2):
		dash_client.append(first[1:])
	elif first[0]==str(3):
		exp_time.append(first[1:])
	elif first[0]==str(4):
		stall_time.append(first[1:])
	elif first[0]==str(5):
		segment_packet.append(first[1:])
	elif first[0]==str(6):
		client_request.append(first[1:])

#for line in exp_time:
#	print(line)

#print(stall_time[0])


# dash_server:  socket_name , time





a = np.asarray(dash_server) # let a be numpy array of dash_server
b = np.unique(a[:,0]) # get the unique socket name
new_server = list()

for i in range(0,len(b)):
	for line in dash_server:
		if line[0] == b[i]:
			tmp = []
			tmp.append(i)
			tmp.append(line[1])
			new_server.append(tmp)	




	
# which user of trace do you want


						


#print(new_server[0])
# new_server: userID, packet send time


# dash_client:  userID , time


client = list()

# segment_packet :  UserID , segment_number , packet_number
# record packet num of each user of each segment

user_j = input_file.split('.')

for i in range(0,int(num_user)):
	file_name = user_j[0]+".csv"
	r_file = open(file_name,'w')
	for line in segment_packet:
		if line[0]==str(i):
			r_file.write(line[2])
			r_file.write('\n')
		



# define which user we want to record  request  receive display


out_new2 = open(output_new2,"w")
#out_get = open(output_get,"w")
#out_stall = open(output_stall,"w")


tmp_server = list()

for i in range(0,int(num_user)):
        tmp=[]
        for line in new_server:
                if line[0]==i:
                        tmp.append(line)
        tmp_server.append(tmp)


for i in range(0,int(num_user)):
	tmp=[]
	for line in dash_client:
		if line[0]==str(i):		
			tmp.append(line)
	client.append(tmp)	

server = list()

for i in range(0, int(num_user)):
	for j in range(0, int(num_user)):
		if len(tmp_server[j])==len(client[i]):
			server.append(tmp_server[j])				





for i in range(0,int(num_user)):
        file_name = user_j[0]+".csv"
        r_file = open(file_name,'r')
        tmp=[]

        for line in r_file:
                tmp.append(int(line))

        record=0
        record_2=1-tmp[0]
        store_size=[]
        store_time=[]
        store_new2=[]
        store_get=[]
        store_size.append(0)
        store_time.append(0)
        total=0
        for line in tmp:
                sum_x = line*1354*8.0
                if record == 0:
                    record_2=1;
                else:
                    record_2 = record
                #print(record_2)
                record += line
                if(float(client[i][record-1][1])-float(client[i][record_2-1][1]))==0:
                         bandwidth_2=0
                else:
                         bandwidth_2 = sum_x / (float(client[i][record-1][1])-float(client[i][record_2-1][1])) # get last - get first
                store_new2.append(bandwidth_2)
                store_get.append(str(client[i][record_2-1][1])) # record get segment time
                total+=line*1354
                store_size.append(total)
                store_time.append(client[i][record-1][1])

        for i in range(0,len(store_new2)):
                out_new2.write(str(store_new2[i]))
                if(i!=len(store_new2)-1):
                        out_new2.write(',')
                else:
                        out_new2.write('\n')		
        '''
        for i in range(0,len(store_get)):
                out_get.write(str(store_get[i]))
                if(i!=len(store_get)-1):
                        out_get.write(',')
                else:
                        out_get.write('\n')
        '''		

# e p_time: user_ID , expected time , real play time
stall=[]
for i in range(0,int(num_user)):
	count=0
	inter_time=0.0
	for line in exp_time:
		if(line[0]==str(i)):
			print(line)
			if(float(line[1])!=float(line[2])):
				count = count+1
				inter_time = inter_time + (float(line[2])-float(line[1]))		
				#print('user{} stall {} times'.format(i,count))
	print('user{} stall {} times'.format(user_j[0][4],count))
	print('total interrupt time %f' % inter_time)
	stall.append(count)
#	out_stall.write(str(count)+','+str(inter_time))
#	out_stall.write('\n')


'''
av_stall = sum(stall)/10
out_stall.write(str(av_stall))
out_stall.write('\n')
'''
