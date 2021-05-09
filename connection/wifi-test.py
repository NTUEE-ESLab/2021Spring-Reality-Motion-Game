#!/usr/bin/env python3

import socket
import json
import numpy
import time
import sys
import os


def main():

    # HOST = '127.0.0.1'  # Standard loopback interface address (localhost)
    HOST = '192.168.50.252'
    PORT = 30004       # Port to listen on (non-privileged ports are > 1023)
    acce = [[] for _ in range(3)]
    gyro = [[] for _ in range(3)]

    acce_labels = ['a_x', 'a_y', 'a_z']
    gyro_labels = ['g_x', 'g_y', 'g_z']
    axis = ['X', 'Y', 'Z']
    array_s = []
    data_count = 0
    error_count = 0
    sample_rate = 0.1
    gyro_scale = 1000

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        # s.setsockopt(socket.SOL_SOCKET, socket.REUSEADDR, 1)
        s.bind((HOST, PORT))
        s.listen()
        conn, addr = s.accept()
        try:
            with conn:
                print('Connected by', addr)
                while True:
                    try:
                        data = conn.recv(8192).decode('utf-8')
                        dec = json.JSONDecoder()
                        pos = 0
                        datas = []

                        # decode several json objects in a single line
                        while not pos == len(str(data)):
                            j, json_len = dec.raw_decode(str(data)[pos:])
                            pos += json_len
                            datas.append(j)

                        for data in datas:
                            data_count += 1
                            json_data = data

                            acce_data = [json_data['ax'],
                                         json_data['ay'],
                                         json_data['az']]

                            gyro_data = [json_data['ang0'] / gyro_scale,
                                         json_data['ang1'] / gyro_scale,
                                         json_data['ang2'] / gyro_scale]

                            # # put data in the array
                            # for i in range(3):
                            #     acce[i].append(acce_data[i])
                            #     gyro[i].append(gyro_data[i])
                            print(
                                f'{data_count}. ACCE: {acce_data[0]}, {acce_data[1]}, {acce_data[2]} GYRO: {gyro_data[0]}, {gyro_data[1]}, {gyro_data[2]}')

                    except json.decoder.JSONDecodeError:
                        print("JSONDecodeError!!!")
                        print(f'data: {data}')
                        error_count += 1
                        continue

        except KeyboardInterrupt:
            print('Interrupted')
            s.close()
            print(f"error_count = {error_count}")


if __name__ == '__main__':
    main()
