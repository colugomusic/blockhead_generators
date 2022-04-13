# blockhead_generators

## Building the plugins
Requires [cmake](https://cmake.org/download/), and a c++ compiler.

To configure for the first time:

`cmake /path/to/blockhead_generators`

To reconfigure:

`cmake .`

To build from command line:

`cmake --build .`

Plugins will be output to `./bin` by default. To output directly to Blockhead's blink directory you can use the `PLUGIN_OUTPUT_DIR` cmake option e.g.

`cmake -DPLUGIN_OUTPUT_DIR=/path/to/Blockhead/blink .`

For a debug/release builds it's

`cmake -DCMAKE_BUILD_TYPE=Debug .`/`cmake -DCMAKE_BUILD_TYPE=Release .` on Linux, or

`cmake --build . --config=Debug`/`cmake --build . --config=Release` on macOS / Windows (or just use the IDE)
