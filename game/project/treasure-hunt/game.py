import socket
import threading
import pygame
import time
from pynput.keyboard import Key, Controller

from utils import load_sprite, print_text, print_status, get_random_position
from utils import get_motion_type, get_motion_message
from models import Player, Asteroid, Treasure


class TreasureHunt:
    """
    The treasure hunt game class.

    The game is divided into 4 phases:
        - handle wifi data
        - handle input
        - process game logic
        - draw

    We create an additional worker thread for listening data from the sensor to prevent blocking call.

    When the game start, it will first wait for connection from the sensor then pop up the game window.
    """
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
        """Initialized game settings."""
        self._init_pygame()
        self._init_game_setup()

    def _init_pygame(self):
        """Initialized pygame object."""
        pygame.init()
        pygame.display.set_caption("Treasure Hunt")

    def _init_game_setup(self):
        """Set up initial game parameters and objects."""
        self.screen = pygame.display.set_mode(
            (self.SCREEN_WIDTH, self.SCREEN_HEIGHT))
        self.background = load_sprite("space", False)
        self.clock = pygame.time.Clock()
        self.font = pygame.font.Font(None, 64)
        self.status_font = pygame.font.Font(None, 32)
        self.player = Player((400, 300), self.bullets.append)
        
        # Render asteroid objects
        for _ in range(3):
            while True:
                position = get_random_position(self.screen)
                if (
                    position.distance_to(self.player.position)
                    > self.MIN_ASTEROID_DISTANCE
                ):
                    break

            self.asteroids.append(Asteroid(position, self.asteroids.append))

        # Render treasure objects
        for _ in range(self.treasure_count):
            position = get_random_position(self.screen)
            self.treasures.append(Treasure(position))

    def start(self):
        """The entry point of the game."""
        # HOST = '192.168.1.254'
        # HOST = '172.20.10.2'
        HOST = '192.168.50.101'
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
                        target=self._get_wifi_data, args=[conn], daemon=True)
                    data_thread.start()

                    # Game loop
                    self.main_loop()

            except KeyboardInterrupt:
                print('Interrupted from keyboard!')

    def main_loop(self):
        """The game loop."""
        while self.playing:
            self._handle_wifi_data()
            self._handle_input()
            self._process_game_logic()
            self._draw()

    def _get_wifi_data(self, conn):
        """Wait for wifi data from the sensor."""
        while self.playing:
            # Get data from connection
            data = conn.recv(1024).decode('utf-8')

            # Lock buffer to prevent race condition
            with self.lock:
                self.motion_buffer.append(data)

    def _handle_motion_type(self):
        """Perform action based on differect motion types."""
        # stand
        if self.motion_type == '0':
            self.player.set_stand()
            self.prev_motion_type = self.motion_type
        # walk
        elif self.motion_type == '1':
            self.player.set_walk()
            self.prev_motion_type = self.motion_type
        # run (to shoot)
        elif self.motion_type == '2':
            self.player.shoot()
            self.prev_motion_type = self.motion_type
        # raise (to run)
        elif self.motion_type == '3':
            self.player.set_run()
            self.prev_motion_type = self.motion_type
        # punch
        elif self.motion_type == '4':
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
        """Process data came from the sensor."""
        # Lock buffer to prevent race condition
        with self.lock:
            # Parse each string from buffer
            for data in self.motion_buffer:
                # Determine motion type
                motion_type = get_motion_type(data)
                self.motion_type = motion_type
                message = get_motion_message(motion_type)
                self.message = message

                # Based on the motion type, do the corresponding action
                self._handle_motion_type()

            # Clear read buffer
            self.motion_buffer = []

    def _handle_input(self):
        """Handle keyboard input (esc) or quit signal."""
        for event in pygame.event.get():
            if event.type == pygame.QUIT or (
                event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE
            ):
                self.playing = False

            elif (
                event.type == pygame.KEYDOWN
                and event.key == pygame.K_SPACE
            ):
                self.player.shoot()

        is_key_pressed = pygame.key.get_pressed()

        # Default key pressed set, for debug purpose
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
        """Calculate position, velocity, and collision for game objects."""

        # Move objects
        for game_object in self._get_game_objects():
            game_object.move(self.screen)

        # Detect collision between asteroids and player
        for asteroid in self.asteroids:
            if asteroid.collides_with(self.player):
                self.message = "You lost!"
                self.playing = False

        # Detect collision between treastures and player
        for treasure in self.treasures:
            if treasure.collides_with(self.player):
                self.treasures.remove(treasure)

        # Detect collision between asteroids and bullets
        for bullet in self.bullets[:]:
            for asteroid in self.asteroids[:]:
                if asteroid.collides_with(bullet):
                    self.asteroids.remove(asteroid)
                    self.bullets.remove(bullet)
                    asteroid.split()
                    break

        # Detect collision between bullets and boundary
        for bullet in self.bullets[:]:
            if not self.screen.get_rect().collidepoint(bullet.position):
                self.bullets.remove(bullet)

        # If no left treasures to collect, then the player wins
        if not self.treasures:
            self.message = "You won!"
            self.playing = False

    def _get_game_objects(self):
        """Get the list contains all game objects."""
        game_objects = [*self.asteroids, *self.bullets, *self.treasures, self.player]

        return game_objects

    def _draw(self):
        """Draw all game objects on the screen."""
        self.screen.blit(self.background, (0, 0))

        for game_object in self._get_game_objects():
            game_object.draw(self.screen)

        if self.message:
            if self.playing:            
                print_status(self.screen, self.message, self.status_font)
            else:
                print_text(self.screen, self.message, self.font)

        pygame.display.flip()

        self.clock.tick(60)
        if not self.playing:
            time.sleep(3)
