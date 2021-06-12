import socket
import re

#HOST = '192.168.50.101'
HOST = '172.20.10.2'
PORT = 65438

def GetType(ins):
    words = ins.decode("utf-8")
    idx = words.find(':',0)
    x = words[idx+1 : idx+2]
    if (x == "0"):
        print("Stand")
    elif (x == "1"):
        print("Walk")
    elif (x == "2"):
        print("Run")
    elif (x == "3"):
        print("Raise")
    elif (x == "4"):
        print("Punch")
    elif (x == "5"):
        print("Right")
    elif (x=="6"):
        print("Left")

if __name__ == '__main__':
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        s.listen()
        conn, addr = s.accept()
        try:  
            with conn:
                print('Connected by', addr)
                while True:
                    data = conn.recv(1024)
#                    print(data)
                    if (data):
                    	GetType(data)
        except KeyboardInterrupt:
          print("Interrupted\n")
          print('closing socket\n')
          s.close()
