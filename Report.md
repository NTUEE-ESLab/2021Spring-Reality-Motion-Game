**2021 Spring ESLab Final Project - Motion**
===

**Authors**:
- 陳映樵 B07901
- 吳宜庭 B07901095

Outline
===
- [Abstract](#abstract)
- [Motivation](#motivation)
- [Implementation](#implementation)
    + [I. System Architecture](#i-system-architecture)
    + [II. System Layering](#ii-system-layering)
      * [1. Data Layer](#1-data-layer)
      * [2. Wifi Layer](#2-wifi-layer)
      * [3. Game Layer](#3-game-layer)
    + [III. Motion Detection](#iii-motion-detection)
    + [IV. Location Service](#iv-location-service)
    + [V. Game Design](#v-game-design)
- [Setup & Usage](#steup--usage)
- [Progress](#progress)
- [Demo](#demo)
- [References](#references)


Abstract
===
In this project, we have created an archaic treasure hunting game played with motion control. We integrated several sensors in STM32 board with our own motion detection algorithm, object class wrapper and multi-threading technique for realization.

Motivation
===
We started from the idea of building a reality game in which players can trigger different tasks based on their locations and complete the tasks by performing some specified movements. 


Implementation
===

### I. System Architecture
![Project Architecture](./images/architecture.png)

### II. System Layering

#### 1. Data Layer

#### 2. WiFi Layer
We implement a wifi wrapper class over the data sensor, which will get motion type from the sensor on a certain interval, then send this information to the game server.

#### 3. Game Layer
We write out game program based on a spaceship shooting game from the web. The player have to move around the screen to find the treasure. If the player collides with asteroids, the player lose the game. If the player collect all treasures, it wins.


### III. Motion Detection

### IV. Location Service

### V. Game Design


Setup & Usage
===
### Board side (mbed compiler)
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



Progress
===
- [MidProposal](https://docs.google.com/presentation/d/1zUISQAgCSKkXEW6G_4c_JL1AxVgvxVhpq2gYIAT18C8/edit?usp=sharing)
- [Progress Report](https://docs.google.com/presentation/d/1QpmcDUexZokhLhzRx4198VfW1-WgqPi4YffoX4_mxNY/edit?usp=sharing)

Demo
===
### Final Project Demo
[![Final Project Demo](http://img.youtube.com/vi/kcvfzlNczBo/0.jpg)](http://www.youtube.com/watch?v=kcvfzlNczBo "Final Demo")

References
===
- 
