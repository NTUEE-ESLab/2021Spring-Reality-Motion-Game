import random

from pygame import Color
from pygame.image import load
from pygame.math import Vector2
from pygame.mixer import Sound


def load_sprite(name, with_alpha=True):
    """Load sprites by given name"""
    path = f"assets/sprites/{name}.png"
    loaded_sprite = load(path)

    if with_alpha:
        return loaded_sprite.convert_alpha()
    else:
        return loaded_sprite.convert()


def load_sound(name):
    """Load sound by given name"""
    path = f"assets/sounds/{name}.wav"
    return Sound(path)


def wrap_position(position, surface, object):
    """
    Determine the next position for an object.
    There are two mode: 
        - One is the object will be constrained in a container.
        - The other one is the object will perform mod operation to 
          go to the opposite side of the screen.
    """
    x, y = position
    w, h = surface.get_size()
    r = object.get_radius()

#    if x < r:
#        x = r
#        object.set_stand()
#    elif x > (w - r):
#        x = w - r
#        object.set_stand()
#
#    if y < r:
#        y = r
#        object.set_stand()
#    elif y > (h - r):
#        y = h - r
#        object.set_stand()
#
#    return Vector2(x, y)
    return Vector2(x % w, y % h)

def asteroid_position(position, surface):
    """The position of the asteroid objects"""
    x, y = position
    w, h = surface.get_size()
    return Vector2(x % w, y % h)


def get_random_position(surface):
    """Get random position among the screen area"""
    return Vector2(
        random.randrange(surface.get_width()),
        random.randrange(surface.get_height()),
    )


def get_random_velocity():
    """Get velocity from 0 to 1 pixel randomly"""
    # speed = random.randint(min_speed, max_speed)
    speed = random.random()
    angle = random.randrange(0, 360)
    return Vector2(speed, 0).rotate(angle)


def print_text(surface, text, font, color=Color("tomato")):
    """Print the win or lose message on the screen"""
    text_surface = font.render(text, True, color)

    rect = text_surface.get_rect()
    rect.center = Vector2(surface.get_size()) / 2

    surface.blit(text_surface, rect)


def print_status(surface, text, font, color=Color("tomato")):
    """Print the motion status on the screen"""
    text_surface = font.render(text, True, color)

    rect = text_surface.get_rect()
    rect.center = Vector2(400, 25)

    surface.blit(text_surface, rect)

def get_motion_type(words):
    """Parse motion type message from the sensor"""
    idx = words.find(':', 0)
    x = words[idx+1 : idx+2]

    return x
    

def get_motion_message(motion_type):
    """Translate motion type to corresponding motion message"""
    message = ''
    
    if (motion_type == "0"):
        message = 'Stand'
    elif (motion_type == "1"):
        message = 'Walk'
    elif (motion_type == "2"):
        message = 'Run'
    elif (motion_type == "3"):
        message = 'Raise'
    elif (motion_type == "4"):
        message = 'Punch'
    elif (motion_type == "5"):
        message = 'Right'
    elif (motion_type == "6"):
        message = 'Left'

    return message
