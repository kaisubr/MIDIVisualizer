## MIDI Visualizer

![Screenshot](screenshot.png)

A small MIDI visualizer, written in C++/OpenGL. Binaries for Windows and Linux are available [here](https://github.com/ekuiter/MIDIVisualizer/releases).

### Usage

On all platforms, you can **run the application by simply double-clicking** on it. You will then be able to select a MIDI file to load. A *Settings* panel now allows you to modify various display parameters. Note that MIDIVisualizer is currently not able to *play* soundtracks, only *display* them.

Press `p` to play/pause the track, `r` to restart at the beginning of the track, and `i` to show/hide the *Settings* panel. 

### Command-Line Interface

You can run the executable from the command-line, specifying a MIDI file to read, along with a state file (INI file which may be exported within MIDI Visualizer), directory, resolution, and frame rate for PNG export.

    ./MIDIVisualizer [path/to/file.mid [state [output_directory width height framerate]]]

### Development

The project is configured using Cmake.

    mkdir build && cd build
    cmake ..
    
Depending on the target you chose in Cmake, you will get either a Visual Studio solution (e.g., `cmake .. -G "Visual Studio 15"`), an Xcode workspace or a set of Makefiles. You can build the main executable using the `MIDIVisualizer`sub-project/target. If you update the images or shaders in the `resources` directory, you will have to repackage them with the executable, by building the `Packaging` sub-project/target. MIDIVisualizer depends on the [GLFW3 library](http://www.glfw.org) and the [Native File Dialog library](https://github.com/mlabbe/nativefiledialog), both are included in the repository and built along with the main executable.

For example, you can compile MIDIVisualizer on Ubuntu (e.g., in a VM with `vagrant init ubuntu/bionic64 && vagrant up && vagrant ssh`) as follows:

    sudo apt-get update
    sudo apt-get -yqq install git cmake xorg-dev gtk+-3.0
    git clone https://github.com/ekuiter/MIDIVisualizer
    cd MIDIVisualizer
    mkdir build
    cd build
    cmake ..
    make

On Windows, you can compile with MSBuild in your PATH (e.g., in `C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\`) as follows:

    git clone https://github.com/ekuiter/MIDIVisualizer
    cd MIDIVisualizer
    mkdir build
    cd build
    cmake .. -G "Visual Studio 15"
    msbuild MIDIVisualizer.sln /p:Configuration=Release