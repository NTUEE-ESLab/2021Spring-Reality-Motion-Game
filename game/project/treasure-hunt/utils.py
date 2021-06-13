import random

from pygame import Color
from pygame.image import load
from pygame.math import Vector2
from pygame.mixer import Sound


def load_sprite(name, with_alpha=True):
    path = f"assets/sprites/{name}.png"
    loaded_sprite = load(path)

    if with_alpha:
        return loaded_sprite.convert_alpha()
    else:
        return loaded_sprite.convert()


def load_sound(name):
    path = f"assets/sounds/{name}.wav"
    return Sound(path)


def wrap_position(position, surface, object):
    x, y = position
    w, h = surface.get_size()
    r = object.get_radius()

    if x < r:
        x = r
        object.set_stand()
    elif x > (w - r):
        x = w - r
        object.set_stand()

    if y < r:
        y = r
        object.set_stand()
    elif y > (h - r):
        y = h - r
        object.set_stand()

    return Vector2(x, y)

def asteroid_position(position, surface):
    x, y = position
    w, h = surface.get_size()
    return Vector2(x % w, y % h)


def get_random_position(surface):
    return Vector2(
        random.randrange(surface.get_width()),
        random.randrange(surface.get_height()),
    )


def get_random_velocity():
    # speed = random.randint(min_speed, max_speed)
    speed = random.random()
    angle = random.randrange(0, 360)
    return Vector2(speed, 0).rotate(angle)


def print_text(surface, text, font, color=Color("tomato")):
    text_surface = font.render(text, True, color)

    rect = text_surface.get_rect()
    rect.center = Vector2(surface.get_size()) / 2

    surface.blit(text_surface, rect)


def print_status(surface, text, font, color=Color("tomato")):
    text_surface = font.render(text, True, color)

    rect = text_surface.get_rect()
    rect.center = Vector2(400, 25)

    surface.blit(text_surface, rect)

def get_motion_type(words):
    idx = words.find(':', 0)
    x = words[idx+1 : idx+2]

    return x
    

def get_motion_message(motion_type):
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