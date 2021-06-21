# Connection

This folder contains several programs to test connection between host computer and sensor (STM32). 

The `wifi-server.py` and `wifi-test.py` use `json` as transmission format. The `server-simple.py` use simple `type:motion` format.

- `wifi-server.py`: A socket program that can receive data from sensor and also plot them in the real time. (From hw program) (version 0)
- `wifi-test.py`: A socket program that can receive data from sensor and print them. (version 1)
- `server-simple.py`: A socket program that test the transmission format that is used in the final project. (version 2)

The `wifi-server.py` and `wifi-test.py` can handle multiple json document arrive at the same time, the technique comes from [this post](https://stackoverflow.com/questions/36967236/parse-multiple-json-objects-that-are-in-one-line/43807246).