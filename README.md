#### How to build

A C++20 compliant compiler is required.

cmake 3.17 or higher is required.

In the project directory
```bash
# <config_name> is the name of one of the directory under resources/
mkdir build && cd build
cmake -DCNT_CONFIG_SUITE=<config_name> ..
make -j4
```

The result is in `cnt_project` directory.
