# 2021eslab_final

## Setup & Usage

### Setup on Mbed compiler
- Configure the Wi-Fi shield to use.

   Edit `mbed_app.json` to include the correct Wi-Fi shield, SSID and password. In our example, `wifi-shield`is "WIFI_ISM43362". Notice that STM32 usually support only 802.11g, so you may have to check your wifi configuration.

   ```
       "config": {
            "wifi-shield": {
                    "help": "Options are WIFI_IDW0XX1",
                    "value": "WIFI_IDW0XX1"
                    },
            "wifi-ssid": {
                "help": "WiFi SSID",
                "value": "\"SSID\""
            },
            "wifi-password": {
                "help": "WiFi Password",
                "value": "\"Password\""
            }
       }
   ```

- Import Sensor library `BSP_B-L475E-IOT01` and `wifi-ism43362`. There is a button under the bottom right corner of the screen.

### Setup for the game
- If you have not installed pygame previously, run `pip install -r requirements.txt` under `game/project`.

### Starting the game
1. Under `game/project`, run `python treasure-hunt`.
2. Make sure STM32 board is held in right hand with hand vertical to ground and press the restart button.
3. The pygame window will automatically pops up when the wifi connection is successfully established.
4. Game instructions:
   * Eat the two treasures and avoid getting hit by aesteroids.
   * Stand: remain in the same spot.
   * Walk: move forward.
   * Right twist: turn right.
   * Run: fire.
   * Raise hand: accelerate.


## Progress
- [MidProposal](https://docs.google.com/presentation/d/1zUISQAgCSKkXEW6G_4c_JL1AxVgvxVhpq2gYIAT18C8/edit?usp=sharing)
- [Progress Report](https://docs.google.com/presentation/d/1QpmcDUexZokhLhzRx4198VfW1-WgqPi4YffoX4_mxNY/edit?usp=sharing)

## Demo
### Final Project Demo
[![Final Project Demo](http://img.youtube.com/vi/kcvfzlNczBo/0.jpg)](http://www.youtube.com/watch?v=kcvfzlNczBo "Final Demo")
### [Final Project Report](./Report.md)
