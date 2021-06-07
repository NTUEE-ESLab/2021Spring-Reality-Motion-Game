import pygame
import socket
import json
import threading

from utils import load_sprite, print_status
from models import Player


class TreasureHunt:
    def __init__(self, width=800, height=600):
        self.playing = True
        self.SCREEN_WIDTH = width
        self.SCREEN_HEIGHT = height
        self.MIN_ASTEROID_DISTANCE = 250
        self.message = ""
        self.bullets = []
        self.data = ""
        self.data_count = 0
        self.error_count = 0
        self.gyro_scale = 1000
        self.motion_buffer = []
        self.lock = threading.Lock()

    def init(self):
        self._init_pygame()
        self._init_game_setup()

    def _init_pygame(self):
        pygame.init()
        pygame.display.set_caption("Treasure Hunt")

    def _init_game_setup(self):
        self.screen = pygame.display.set_mode(
            (self.SCREEN_WIDTH, self.SCREEN_HEIGHT))
        self.background = load_sprite("space", False)
        self.clock = pygame.time.Clock()
        self.font = pygame.font.Font(None, 64)
        self.status_font = pygame.font.Font(None, 12)
        self.player = Player((400, 300), self.bullets.append)

    def start(self):
        HOST = '192.168.1.254'
        # Port to listen on (non-privileged ports are > 1023)
        PORT = 30006

        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            # To reuse the same port for each connection
            s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            s.bind((HOST, PORT))
            s.listen()
            conn, addr = s.accept()
            try:
                with conn:
                    print('Connected by', addr)
                    self.init()

                    # Use another to listen data from motion sensor
                    data_thread = threading.Thread(
                        target=self._get_wifi_data, args=[conn])
                    data_thread.start()

                    # Game loop
                    # game_thread = threading.Thread(target=self.main_loop)
                    # game_thread.start()
                    self.main_loop()

            except KeyboardInterrupt:
                print('Interrupted')
                s.close()
                print(f"error_count = {self.error_count}")

    def main_loop(self):
        while self.playing:
            self._handle_wifi_data()
            self._handle_input()
            self._process_game_logic()
            self._draw()

    def _get_wifi_data(self, conn):
        while self.playing:
            # Get data from connection
            data = conn.recv(8192).decode('utf-8')

            # Lock buffer to prevent race condition
            with self.lock:
                self.motion_buffer.append(data)

    def _handle_wifi_data(self):
        # Lock buffer to prevent race condition
        with self.lock:
            # Parse each string from buffer
            for data in self.motion_buffer:
                try:
                    # data = conn.recv(8192).decode('utf-8')
                    dec = json.JSONDecoder()
                    pos = 0
                    datas = []

                    # decode several json objects in a single line
                    while not pos == len(str(data)):
                        j, json_len = dec.raw_decode(str(data)[pos:])
                        pos += json_len
                        datas.append(j)

                    for data in datas:
                        self.data_count += 1
                        json_data = data

                        acce_data = [json_data['ax'],
                                     json_data['ay'],
                                     json_data['az']]

                        gyro_data = [json_data['ang0'] / self.gyro_scale,
                                     json_data['ang1'] / self.gyro_scale,
                                     json_data['ang2'] / self.gyro_scale]

                        val = json_data['val']
                        all = json_data['all']
                        diff = json_data['diff']

                        # # put data in the array
                        # for i in range(3):
                        #     acce[i].append(acce_data[i])
                        #     gyro[i].append(gyro_data[i])
                        message = f'{self.data_count}. ACCE -- X:{acce_data[0]}, Y:{acce_data[1]}, Z:{acce_data[2]} GYRO -- X:{gyro_data[0]}, Y:{gyro_data[1]}, Z:{gyro_data[2]}, Val: {val}, All: {all}, Diff: {diff}'
                        # print(message)
                        self.message = message
                        # print(self.message)

                except json.decoder.JSONDecodeError:
                    print("JSONDecodeError!!!")
                    print(f'data: {data}')
                    self.error_count += 1

            # Clear read buffer
            self.motion_buffer = []

    def _handle_input(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT or (
                event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE
            ):
                self.playing = False
                # quit()

            elif (
                event.type == pygame.KEYDOWN
                and event.key == pygame.K_SPACE
            ):
                self.player.shoot()

        is_key_pressed = pygame.key.get_pressed()

        # Handle rotation
        if is_key_pressed[pygame.K_RIGHT]:
            self.player.rotate(clockwise=True)
        elif is_key_pressed[pygame.K_LEFT]:
            self.player.rotate(clockwise=False)

        # Handle acceleration
        if is_key_pressed[pygame.K_UP]:
            self.player.accelerate()
        elif is_key_pressed[pygame.K_DOWN]:
            self.player.decelerate()

    def _process_game_logic(self):
        for game_object in self._get_game_objects():
            game_object.move(self.screen)

        # self.message = f'POS: {self.player.get_position()}, SPEED: {self.player.get_velocity()}'

        # for asteroid in self.asteroids:
        #     if asteroid.collides_with(self.spaceship):
        #         self.spaceship = None
        #         self.message = "You lost!"
        #         break

        # for bullet in self.bullets[:]:
        #     for asteroid in self.asteroids[:]:
        #         if asteroid.collides_with(bullet):
        #             self.asteroids.remove(asteroid)
        #             self.bullets.remove(bullet)
        #             asteroid.split()
        #             break

        for bullet in self.bullets[:]:
            if not self.screen.get_rect().collidepoint(bullet.position):
                self.bullets.remove(bullet)

    def _get_game_objects(self):
        # game_objects = [*self.asteroids, *self.bullets]

        game_objects = [*self.bullets, self.player]

        return game_objects

    def _draw(self):
        self.screen.blit(self.background, (0, 0))

        for game_object in self._get_game_objects():
            game_object.draw(self.screen)

        if self.message:
            print_status(self.screen, self.message, self.status_font)

        pygame.display.flip()

        self.clock.tick(60)
