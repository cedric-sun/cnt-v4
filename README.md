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

Adjust the `CNT_CONFIG_SUITE` variable in the `CMakeLists.txt` to choose which config suite to be used. Name of available config suites are the same as sub-directories name in the `resources/` directory.

The result is in `build/cnt_project` directory.
