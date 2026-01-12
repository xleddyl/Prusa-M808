# COMPILE ->

`python3 -m venv ./venv`
`. ./venv/bin/activate`
`./utils/bootstrap.py`

`mkdir build && cd build && cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/AvrGcc.cmake && ninja`
