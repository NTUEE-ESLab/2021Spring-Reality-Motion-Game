# A Simple Treasure Hunt Game

## Usage
1. Run `python treasure-hunt` under the `project` folder directly. Don't run under `treasure-hunt` folder with `python __main__.py`.
2. Press reset button on STM32 and wait for connection.

## Game instructions
- Goal: Get two treasures and avoid asteroids
- Stand: Just stand still to remain in the same place
- Walk: Sway hand and walk normally
- Accelerate: Raise hand to add some speed
- Direction: Right twist the wrist to turn right
- Shoot: Run to Shoot

## Calibration
To recalibrate, hold hands (and the STM32 board) loosely by your side and press the user button.

## Architecture
- game/project/
    - assets/
        - sounds/: The folder that holds the sound files.
        - sprites/: The folder that holds the image sprites.
    - treasure-hunt/
        - \_\_main\_\_.py: The entry point of our program. **Don't** run it directly.
        - game.py: The main game class.
        - models.py: Contains several game object classes.
        - utils.py: Several utils functions.
    - README.md: Contains some instructions and explanation of how to run the game.
    - requirements.txt: Run `pip install -r requirements.txt` in the shell. Actually we only need pygame module in our game.
    
