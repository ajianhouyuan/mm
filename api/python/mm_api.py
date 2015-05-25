import socket
import time
import os

class mmsocket():
	def __init__(self, ip = '192.168.177.131', port=1234):
		self.read_buffer = None
		self.write_buffer = None
		self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.sock.connect((ip,port))

	def __read(self):
		n = 1
		total_len = 0
		split_zero = -1
		recv_data = ""

		while True:
			recv_data = recv_data + self.sock.recv(1024*n)
			print "recv_data     = ",recv_data
			print "recv_data_len = ",len(recv_data)
			split_zero = recv_data.find('\0')
			if (split_zero != -1):
				total_len = int(recv_data[0:split_zero])
			else:
				continue

			if (total_len == len(recv_data)-(split_zero+1)):
				#print "break"
				break

			if (n*1024 < 1024*1024):
				n=n*2

		return recv_data

	def __write(self,send_data):
		self.sock.send(send_data)


	def is_error(self, recv_data):
		body = recv_data.split('\0')[1]
		if (body[0:4] == "-ERR"):
			return body
		else:
			return None

	def s_get(self,key):
		if key == None or key == "":
			return (-1,"-ERR PARAMETER_ERROR")

		send_data = '3\0' + 'get\0' + str(len(str(key))) + '\0' + str(key) + '\0'
		send_data = str(len(send_data)) + '\0' + send_data
		print "s_get_data    = ",send_data
		self.__write(send_data)
		recv_data = self.__read()

		if (self.is_error(recv_data)!=None):
			return (-1,self.is_error(recv_data))

		split_data = recv_data.split('\0')
		if (len(split_data) == 5):
			return (0,split_data[3])
		else:
			return (-1,"-ERR PARSE_ERROR")

	def s_set(self,key, value):
		if key == None or key == "" or value == None or value == "":
			return (-1,"-ERR PARAMETER_ERROR")

		send_data = '3\0' + 'set\0' + str(len(str(key))) + '\0' + str(key) + '\0' + str(len(str(value))) + '\0' + str(value) + '\0'
		send_data = str(len(send_data)) + '\0' + send_data
		print "s_set_data    = ",send_data
		self.__write(send_data)
		recv_data = self.__read()

		if (self.is_error(recv_data)!=None):
			return (-1,self.is_error(recv_data))

		split_data = recv_data.split('\0')
		if (len(split_data) == 3):
			return (0,split_data[1])
		else:
			return (-1,"-ERR PARSE_ERROR")

	def s_del(self,key):
		if key == None or key == "":
			return (-1,"-ERR PARAMETER_ERROR")

		send_data = '3\0' + 'del\0' + str(len(str(key))) + '\0' + str(key) + '\0'
		send_data = str(len(send_data)) + '\0' + send_data
		print "s_del_data    = ",send_data
		self.__write(send_data)
		recv_data = self.__read()

		if (self.is_error(recv_data)!=None):
			return (-1,self.is_error(recv_data))

		split_data = recv_data.split('\0')
		if (len(split_data) == 3):
			return (0,split_data[1])
		else:
			return (-1,"-ERR PARSE_ERROR")


if __name__ == '__main__':
	test_value = "AJ"*1024*512

	test = mmsocket()
	for i in range(0,500):
		print "======================",i,"======================"
		set_result = test.s_set(i,test_value)
		print "SET_RESLUT    = ",set_result

	for i in range(0,500):
		print "======================",i,"======================"
		get_result = test.s_get(i)
		print "GET_RESLUT    = ",get_result

	for i in range(0,500):
		print "======================",i,"======================"
		del_result = test.s_del(i)
		print "DEL_RESLUT    = ",del_result

	exit()

	test = mmsocket()
	for i in range(0,10):
		print "======================",i,"======================"

		set_result = test.s_set("1","34200")
		print "SET_RESLUT    = ",set_result

		get_result = test.s_get("1")
		print "GET_RESLUT    = ",get_result

		del_result = test.s_del("1")
		print "DEL_RESLUT    = ",del_result



# sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# sock.connect(('192.168.177.131',1234))
#
# start_time = time.time()
# print "start time",start_time
# for i in range(0,100):
# 	packet_data = "3\0" + "set\0" + "4\0" + "test\0" + "1000000\0"# + "12345\0";
# 	for j in range(0,10000):
# 		packet_data = packet_data + "10000sdlfkajsldkfjsdlfkajsldkfkajsldkfjsdlfkajsldkfjsdlfkajsldkfjsdlfkajsldkfjsdlfkfjsdl@@@@@@@@@@@@"
# 	packet_data = packet_data + '\0'
#
#
# 	sock.send(str(len(packet_data)) + "\0" +packet_data )
# 	sock.recv(1002400)
#
# 	#get test
# 	packet_data = "3\0" + "get\0" + "4\0" + "test\0"
# 	sock.send(str(len(packet_data)) + "\0" +packet_data )
# 	sock.recv(10024000)
# 	#print sock.recv(102400)
#
# 	#del test
# 	packet_data = "3\0" + "del\0" + "4\0" + "test\0"
# 	sock.send(str(len(packet_data)) + "\0" +packet_data )
# 	print sock.recv(1002400)
#
# 	#get test
# 	packet_data = "3\0" + "get\0" + "4\0" + "test\0"
# 	sock.send(str(len(packet_data)) + "\0" +packet_data )
# 	print sock.recv(1002400)
#
#
# #sock.close()
# print "end time",time.time();
# print "___time", time.time() - start_time
# exit(0)
# print "exit"
#
#
# start_time = time.time()
# print "start time",start_time
# for x in range(1,2):
# 	sock.send("13\0" + "3\0" + "get\0" + "4\0test\0")
# 	data = sock.recv(1024)
# 	print data
# print "end time",time.time();
# print "___time", time.time() - start_time
#
# sock.close()
#
# print "exit"


