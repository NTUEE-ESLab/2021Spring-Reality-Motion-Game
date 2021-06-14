import socket
import json
import threading
import pygame
from pynput.keyboard import Key, Controller

from utils import load_sprite, print_status, get_random_position
from utils import get_motion_type, get_motion_message
from models import Player, Asteroid, Treasure


class TreasureHunt:
    def __init__(self, width=800, height=600):
        self.playing = True
        self.SCREEN_WIDTH = width
        self.SCREEN_HEIGHT = height
        self.MIN_ASTEROID_DISTANCE = 250
        self.message = ""
        self.asteroids = []
        self.bullets = []
        self.treasures = []
        self.data = ""
        self.data_count = 0
        self.error_count = 0
        self.gyro_scale = 1000
        self.motion_buffer = []
        self.lock = threading.Lock()
        self.motion_type = ''
        self.prev_motion_type = ''
        self.keyboard = Controller()
        self.treasure_count = 2

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
        self.status_font = pygame.font.Font(None, 32)
        self.player = Player((400, 300), self.bullets.append)
        

        for _ in range(3):
            while True:
                position = get_random_position(self.screen)
                if (
                    position.distance_to(self.player.position)
                    > self.MIN_ASTEROID_DISTANCE
                ):
                    break

            self.asteroids.append(Asteroid(position, self.asteroids.append))

        for _ in range(self.treasure_count):
            position = get_random_position(self.screen)
            self.treasures.append(Treasure(position))

    def start(self):
        HOST = '192.168.1.254'
        # Port to listen on (non-privileged ports are > 1023)
        PORT = 30006

        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            # To reuse the same port for each connection
            s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            s.bind((HOST, PORT))
            s.listen(5)
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
            data = conn.recv(1024).decode('utf-8')

            # Lock buffer to prevent race condition
            with self.lock:
                self.motion_buffer.append(data)

    def _handle_motion_type(self):
        # stand
        if self.motion_type == '0':
            self.player.set_stand()
            self.prev_motion_type = self.motion_type
        # walk
        elif self.motion_type == '1':
            self.player.set_walk()
            self.prev_motion_type = self.motion_type
        # run
        elif self.motion_type == '2':
            self.player.set_run()
            self.prev_motion_type = self.motion_type
        # raise
        elif self.motion_type == '3':
            self.prev_motion_type = self.motion_type
        # punch
        elif self.motion_type == '4':
            # To prevent over shooting
            if self.prev_motion_type != self.motion_type:
                self.player.shoot()
            self.prev_motion_type = self.motion_type
        # right
        elif self.motion_type == '5':
            # To prevent over rotating
            if self.prev_motion_type != self.motion_type:
                self.player.rotate(angle=45, clockwise=True)
            self.prev_motion_type = self.motion_type
        # left
        elif self.motion_type == '6':
            # To prevent over rotating
            if self.prev_motion_type != self.motion_type:
                self.player.rotate(angle=45, clockwise=False)
            self.prev_motion_type = self.motion_type


    def _handle_wifi_data(self):
        # Lock buffer to prevent race condition
        with self.lock:
            # Parse each string from buffer
            for data in self.motion_buffer:
                motion_type = get_motion_type(data)
                self.motion_type = motion_type
                message = get_motion_message(motion_type)
                self.message = message

                # Based on the motion type
                self._handle_motion_type()

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

        for asteroid in self.asteroids:
            if asteroid.collides_with(self.player):
                self.message = "You lost!"
                self.playing = False

        for treasure in self.treasures:
            if treasure.collides_with(self.player):
                self.treasures.remove(treasure)

        for bullet in self.bullets[:]:
            for asteroid in self.asteroids[:]:
                if asteroid.collides_with(bullet):
                    self.asteroids.remove(asteroid)
                    self.bullets.remove(bullet)
                    asteroid.split()
                    break

        for bullet in self.bullets[:]:
            if not self.screen.get_rect().collidepoint(bullet.position):
                self.bullets.remove(bullet)

    def _get_game_objects(self):
        # game_objects = [*self.asteroids, *self.bullets]

        game_objects = [*self.asteroids, *self.bullets, *self.treasures, self.player]

        return game_objects

    def _draw(self):
        self.screen.blit(self.background, (0, 0))

        for game_object in self._get_game_objects():
            game_object.draw(self.screen)

        if self.message:
            print_status(self.screen, self.message, self.status_font)

        pygame.display.flip()

        self.clock.tick(60)
