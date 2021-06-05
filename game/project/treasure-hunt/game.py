import pygame

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

    def init(self):
        self._init_pygame()
        self._init_game_setup()

    def _init_pygame(self):
        pygame.init()
        pygame.display.set_caption("Space Rocks")

    def _init_game_setup(self):
        self.screen = pygame.display.set_mode(
            (self.SCREEN_WIDTH, self.SCREEN_HEIGHT))
        self.background = load_sprite("space", False)
        self.clock = pygame.time.Clock()
        self.font = pygame.font.Font(None, 64)
        self.status_font = pygame.font.Font(None, 20)
        self.player = Player((400, 300), self.bullets.append)

    def main_loop(self):
        while self.playing:
            self._handle_input()
            self._process_game_logic()
            self._draw()

    def _handle_input(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT or (
                event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE
            ):
                quit()

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

        self.message = f'POS: {self.player.get_position()}, SPEED: {self.player.get_velocity()}'

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
