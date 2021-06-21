from pygame.math import Vector2
from pygame.transform import rotozoom

from utils import get_random_velocity, load_sound, load_sprite, wrap_position, asteroid_position

UP = Vector2(0, -1)


class GameObject:
    """
    The basic game object, which implement several methods:
        - draw
        - move
        - collides_with

    The game process will call these methods for rendering objects.
    """
    def __init__(self, position, sprite, velocity):
        # the position and the velocity are vectors, so they should perform vector operations
        self.position = Vector2(position)
        self.sprite = sprite
        # consider a game object as a circle 
        self.radius = sprite.get_width() / 2
        self.velocity = Vector2(velocity)

    def draw(self, surface):
        """Draw the object onto the screen"""
        blit_position = self.position - Vector2(self.radius)
        surface.blit(self.sprite, blit_position)

    def move(self, surface):
        """Move the object based on the velocity and position"""
        self.position = wrap_position(
            self.position + self.velocity, surface, self)

    def collides_with(self, other_obj):
        """Detect if this object is collided with other object"""
        distance = self.position.distance_to(other_obj.position)
        return distance < self.radius + other_obj.radius


class Player(GameObject):
    """
    The player object class. 
    
    This class has several additional methods, including:
        - several "getter" and "setter" methods
        - rotate
        - accelerate
        - decelerate
        - shoot

    , which makes this class controllable by player.
    """
    def __init__(self, position, create_bullet_callback):
        self.create_bullet_callback = create_bullet_callback
        self.laser_sound = load_sound("laser")
        self.direction = Vector2(UP)
        # parameters
        self.ACCELERATION = 0.25
        self.WALK_SPEED = 1
        self.RUN_SPEED = 2
        self.BULLET_SPEED = 3

        super().__init__(position, load_sprite("spaceship"), Vector2(0))

    def get_radius(self):
        """Get the radius of the object"""
        return self.radius

    def get_position(self):
        """Get the position of the object"""
        return self.position

    def get_velocity(self):
        """Get the velocity of the object"""
        return self.velocity

    def rotate(self, angle=3, clockwise=True):
        """
        Rotate the player.

        Parameters:
        ----------
        angle:
            the degree to rotate, the default is 3 degrees

        clockwise:
            rotate clockwisely or counter-clockwisely
        """
        sign = 1 if clockwise else -1
        angle = angle * sign
        self.direction.rotate_ip(angle)

    def draw(self, surface):
        angle = self.direction.angle_to(UP)
        rotated_surface = rotozoom(self.sprite, angle, 1.0)
        rotated_surface_size = Vector2(rotated_surface.get_size())
        blit_position = self.position - rotated_surface_size * 0.5
        surface.blit(rotated_surface, blit_position)

    def accelerate(self):
        """Accelerate the object by acceleration parameters"""
        self.velocity += self.direction * self.ACCELERATION

    def decelerate(self):
        """Decelerate the object by deceleration parameters"""
        self.velocity -= self.direction * self.ACCELERATION

    def set_walk(self):
        """Set the player to the walk state"""
        self.velocity = self.direction * self.WALK_SPEED

    def set_run(self):
        """Set the player to the run state"""
        self.velocity = self.direction * self.RUN_SPEED

    def set_stand(self):
        """Set the player to the stationary state"""
        self.velocity = Vector2(0, 0)

    def shoot(self):
        """
        Perform shoot operation of the player, it will add a bullet the the bullet list.
        The player class contains the append call back function when initialized.
        """
        bullet_velocity = self.direction * self.BULLET_SPEED + self.velocity
        bullet = Bullet(self.position, bullet_velocity)
        self.create_bullet_callback(bullet)
        self.laser_sound.play()


class Spaceship(GameObject):
    """The original spaceship object from the tutorial."""
    def __init__(self, position, create_bullet_callback):
        self.create_bullet_callback = create_bullet_callback
        self.laser_sound = load_sound("laser")
        # Make a copy of the original UP vector
        self.direction = Vector2(UP)
        self.MANEUVERABILITY = 3
        self.ACCELERATION = 0.25
        self.BULLET_SPEED = 3

        super().__init__(position, load_sprite("spaceship"), Vector2(0))

    def rotate(self, clockwise=True):
        sign = 1 if clockwise else -1
        angle = self.MANEUVERABILITY * sign
        self.direction.rotate_ip(angle)

    def draw(self, surface):
        angle = self.direction.angle_to(UP)
        rotated_surface = rotozoom(self.sprite, angle, 1.0)
        rotated_surface_size = Vector2(rotated_surface.get_size())
        blit_position = self.position - rotated_surface_size * 0.5
        surface.blit(rotated_surface, blit_position)

    def accelerate(self):
        self.velocity += self.direction * self.ACCELERATION

    def shoot(self):
        bullet_velocity = self.direction * self.BULLET_SPEED + self.velocity
        bullet = Bullet(self.position, bullet_velocity)
        self.create_bullet_callback(bullet)
        self.laser_sound.play()


class Asteroid(GameObject):
    """
    The asteriod object. If one of the asteroid collides with the player, the game ends.
    """
    def __init__(self, position, create_asteroid_callback, size=3):
        self.create_asteroid_callback = create_asteroid_callback
        self.size = size

        # for the split method
        size_to_scale = {
            3: 1,
            2: 0.5,
            1: 0.25,
        }
        scale = size_to_scale[size]
        sprite = rotozoom(load_sprite("asteroid"), 0, scale)

        super().__init__(
            position, sprite, get_random_velocity()
        )

    def move(self, surface):
        self.position = asteroid_position(
            self.position + self.velocity, surface)

    def split(self):
        """The asteroid will split if hit by the bullet."""
        if self.size > 1:
            for _ in range(2):
                asteroid = Asteroid(
                    self.position, self.create_asteroid_callback, self.size - 1
                )
                self.create_asteroid_callback(asteroid)


class Bullet(GameObject):
    """The bullet class."""
    def __init__(self, position, velocity):
        super().__init__(position, load_sprite("bullet"), velocity)

    def move(self, surface):
        self.position = self.position + self.velocity

class Treasure(GameObject):
    """The treasure class."""
    def __init__(self, position):
        super().__init__(position, load_sprite("treasure"), Vector2(0))

    def move(self, surface):
        pass

