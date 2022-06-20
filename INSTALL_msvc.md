# Compiling in Visual Studio for Windows

Author's note:
- To simplify the build process, `vcpkg` was used. These steps worked for me, and this documentation should be updated as new/updated methods are discovered/refined.
- Feel free to use a different installation path for `kq-fork` and `vcpkg` to fit your environment.
- I chose to use Git Bash, which uses Linux notation for paths (using `/` forward slashes instead of `\` backslashes between directories) and other Linux/Unix-centric features.

1. `cmake` will need to be in your PATH.
    - My version was older than the [CMakeLists.txt](CMakeLists.txt) file required, so I had to install an updated version to `C:\Program Files\CMake` and add its `bin` directory to my PATH:

        ```bash
        ~$ # Change 'to_add' below as necessary; this special notation only appends to PATH if not already there:
        ~$ to_add="/c/Program Files/CMake/bin" ; [[ :"$PATH": =~ :"$to_add": ]] || PATH="$PATH:$to_add"
        ```

1. Clone the [vcpkg](https://github.com/Microsoft/vcpkg/) repo and install it according to its [Getting Started](https://vcpkg.io/en/getting-started.html) page.
    1. The steps below followed the instructions for using vcpkg with Visual Studio.
    1. I installed mine to `/c/Users/user/soure/repos/vcpkg/`:
        ```bash
        ~$ mkdir -p "$HOME/source/repos/"
        ~$ cd "$HOME/source/repos/"
        ~/source/repos$ git clone https://github.com/Microsoft/vcpkg.git
        ~/source/repos$ to_add="$HOME/source/repos/vcpkg" ; [[ :"$PATH": =~ :"$to_add": ]] || PATH="$PATH:$to_add"
        ~/source/repos$ ./vcpkg/bootstrap-vcpkg.bat -disableMetrics # the '-disableMetrics' flag is optional
        ```
1. Install the necessary libraries: `libpng`, `lua`, `sdl2-mixer`, `sdl2`, `tinyxml2`, `zlib`:
    1. The `sdl2-mixer` package should include support for, at a minimum, `libmodplug`.
    1. I'm on 64-bit Windows, so I used the `--triplet x64-windows` flag; adjust as needed.

        ```bash
        $ vcpkg install --triplet x64-windows libpng lua sdl2 sdl2-mixer[libmodplug,nativemidi] tinyxml2 zlib
        $ vcpkg integrate install # Without this step, I get "not found" errors for the SDL2, Lua, etc. packages within Visual Studio later
        ```
1. Clone `kq-fork`.
    ```bash
    $ git clone https://github.com/OnlineCop/kq-fork
    ```
1. Within the `kq-fork/` directory, use `cmake` to generate the files under the `build/` directory:
    ```bash
    $ cd kq-fork
    $ cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE="../vcpkg/scripts/buildsystems/vcpkg.cmake" # or the path where vcpkg installed its 'vcpkg.cmake' file
    ```
    1. NOTE: Visual Studio uses the CMake-generated files from `kq-fork/build/` to generate its solution, but it builds into the `kq-fork/out/` directory.
    1. If `CMakeLists.txt` has the `KQ_DATADIR` value set to `"."` (current directory), the project will *build* but will halt during *execution* because it can't locate the resources relative to the `kq-fork.exe` executable.
    1. During development, I changed this value to the project root so VS2022 could find its resources:
        ```cmake
        #set(KQ_DATADIR "." CACHE STRING "Where the data dirs are going to be installed")
        set(KQ_DATADIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE PATH "Where the data dirs are going to be installed")
        ```
1. You should now be able to open Visual Studio and open the entire `kq-fork` directory.
    - I opened VS2022 to `C:\Users\kqlives\source\repos\kq-fork`.
1. At the top of Visual Studio is a dropdown that initially says "Select Startup Item..." Selecting `kq-fork.exe` within the dropdown allows you to build and debug the executable.

## Troubleshooting

1. If Visual Studio complains about packages that you installed using `vcpkg` not being found, run its integration script: `vcpkg.exe integrate install` to make them discoverable by IntelliSense.
