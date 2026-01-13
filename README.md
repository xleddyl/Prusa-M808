# Prusa M808
## How to compile

1. `python3 -m venv ./venv`
2. `. ./venv/bin/activate`
3. `./utils/bootstrap.py`

4. `mkdir build`
5. `cd build`
6. `cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/AvrGcc.cmake`
7. `ninja`

## Marlin docs

https://marlinfw.org/docs/gcode/M808.html
