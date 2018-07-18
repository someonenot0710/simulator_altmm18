import sys
import pandas as pd
import math

input_file = sys.argv[1] # file name
in_row = sys.argv[2]
in_col = sys.argv[3]


in_file = open(input_file,'r')
udp_server = list()
udp_client = list()
assumed_packet = list() 
packet_loss = list()

# coaster coaster2 diving drive game landscape pacman panel ride sport

'''
0: 49  coaster
1: 39  coaster2
2: 4 diving
3: 2 drive
4: 29 game
5: 37 landscape 
6: 50 pacman
7: 19 panel 
8: 43 ride
9: 28 sport
'''

video = ['coaster', 'coaster2', 'diving', 'drive', 'game', 
	'landscape', 'pacman', 'panel', 'ride', 'sport']
user_number = ['49','39','04','02','29','37','50','19','43','28']

for line in in_file:
	line = line.strip()
	first = line.split(',')
	if first[0]=='u' and first[1]==str(1):
		udp_server.append(first[2:])
	elif first[0]=='u' and first[1]==str(2):
		udp_client.append(first[2:])
	elif first[0]=='u' and first[1]==str(3):
		packet_loss.append(first[2:])
	elif first[0]=='u' and first[1]==str(4):
		assumed_packet.append(first[2:])


total_ass = 0;
for line in assumed_packet:
	total_ass+= int (line[2]) 
loss = 0;
for line in packet_loss:
	loss+=int(line[1])


print('assume received packet %d, client receive %d, total loss %d' % (total_ass, len(udp_client), loss))

if (loss+len(udp_client) != total_ass):
	print('packets are not the same')


sever_start=0
server_end=0
client_start=0
client_end=0

Throughput = []
segment=0
## bandwidth consumption

while(segment<60): 
	packet_num = int(assumed_packet[segment][2]) # packet number
	s_loss = 0
	for line in packet_loss:
		if int(line[0]) == segment:
			s_loss = int(line[1])  # check if there is lost of this segment

	received_packet = packet_num - s_loss # calculate how many packet did client get

	server_send=0  # the packet number that server sent

	for line in udp_server:
		if int(line[0]) == segment:
			server_send+=1

	
	client_end += received_packet	
	total_time = 0.0
	for i in range(client_start, client_end):
		total_time+= (float(udp_client[i][1])-float(udp_server[sever_start-client_start+i][1]))

	thr = received_packet*1354*8.0 / total_time
	if packet_num == 1 and int(assumed_packet[segment][3])==0:
		Throughput.append(0)
	else:
		Throughput.append(thr)

	client_start+=received_packet
	sever_start+=server_send 
	segment+=1
print(Throughput)

dr_root_dir = '/data3/jerry/dr-predict/trace/'
cur_root_dir = '/data3/jerry/current-predict/size_no_header/'
user = 'user'+user_number[int(input_file.split('.')[0][4])]
video_name = video[int(input_file.split('.')[0][4])]

#dr_file_name = dr_root_dir+in_row+'x'+in_col+'/'+video_name+'_'+user+'_'+in_row+'x'+in_col+'_dr_prob'
#cur_file_name = cur_root_dir+in_row+'x'+in_col+'/'+video_name+'_'+user+'_'+'0.000_1_cur'

dr_size_dir = '/data3/jerry/udp_size/'
dr_size_qp28_name =  dr_size_dir+in_row+'x'+in_col+'/'+'qp28/'+video_name+'_'+user+'_'+'0_1_udp_size_qp28'


dr_size_qp32_name = dr_size_dir+in_row+'x'+in_col+'/'+'qp32/'+video_name+'_'+user+'_'+'0_1_udp_size_qp32'

dr_size_qp36_name = dr_size_dir+in_row+'x'+in_col+'/'+'qp36/'+video_name+'_'+user+'_'+'0_1_udp_size_qp36'


df_size_qp28 = pd.read_csv(dr_size_qp28_name, delim_whitespace=True, header=None).values

df_size_qp32 = pd.read_csv(dr_size_qp32_name, delim_whitespace=True, header=None).values

df_size_qp36 = pd.read_csv(dr_size_qp36_name, delim_whitespace=True, header=None).values


df_dr = [] # udp trace  
segment=0
while(segment<60):
	tmp=[]
	for i in range(0, int(in_row)*int(in_col)):
		if int(df_size_qp28[segment][i]) > 0 :
			tmp.append(1)
		else:
			tmp.append(0)
	df_dr.append(tmp)
	segment+=1

cur_size_name_qp28 = cur_root_dir+in_row+'x'+in_col+'/cur28/'+video_name+'_'+user+'_'+'0_1_cur_size'


print(cur_size_name_qp28)


#df_dr = pd.read_csv(dr_file_name, delim_whitespace=True, header=None).values
df_cur_size = pd.read_csv(cur_size_name_qp28, delim_whitespace=True, header=None).values

segment=0
df_cur=[]
while(segment<60):
        tmp=[]
        for i in range(0, int(in_row)*int(in_col)):
                if int(df_cur_size[segment][i]) > 0 :
                        tmp.append(1)
                else:
                        tmp.append(0)
        df_cur.append(tmp)
        segment+=1
	

dr_trace=[]
segment=0

while(segment<60):
#	print(segment)
	quality = int(assumed_packet[segment][1]) # 1 high-qp28  2 medium-qp32 3 low-qp32 
	loss = 0
	for line in packet_loss:
		if int(line[0]) == segment:
			loss = int(line[1])
	tmp = []
	if loss==0:
		for i in range (0, int(in_row)*int(in_col)):
			if loss==0:
				if int(df_dr[segment][i])==1:
					tmp.append(quality)
				else:
					tmp.append(0)

	else:
		receive_packet = int(assumed_packet[segment][2]) - loss
		if quality ==1:
			size = df_size_qp28[segment];
		elif quality==2:
			size = df_size_qp32[segment];
		elif quality==3:
			size = df_size_qp36[segment];
		index=0
		for j in range(0, int(in_row)*int(in_col)):
			t=0
			for num in range(0,j):
				t+=size[num]
			if math.ceil(t/1346) > receive_packet:
				index=j
				break
		for k in range (0, index-1):
			if int(df_dr[segment][k])==1:
				tmp.append(quality)
			else:
				tmp.append(0)
		for k in range (index-1, int(in_row)*int(in_col)):
			tmp.append(0)
			
	dr_trace.append(tmp)
	segment+=1



overlap=[]
segment=0
while(segment<60):
	tmp=[]
	for i in range (int(in_row)*int(in_col)):
		if int(df_cur[segment][i]) > 0:
			tmp.append(1)
		elif dr_trace[segment][i] > 0:
			tmp.append(int(dr_trace[segment][i]))
		else:
			tmp.append(0)

		if int(df_cur[segment][i]) > 0 and dr_trace[segment][i] > 0:
			print('error, tile conflit segment: %d,  index: %d'%(segment,i))
			sys.exit(0)
		
	overlap.append(tmp)
	segment+=1

#print(dr_trace[59])



out_quality = video_name+'_'+user+'_0_1_quality'

d = pd.DataFrame(overlap)
d.to_csv(out_quality, index=False, header=None, sep= ' ')











