# MIDI Visualizer

A small MIDI visualizer, written in C++/OpenGL. 

![Result image](result1.png)  

## Usage

On all platforms, you can now **run the application by simply double-clicking** on it. You will then be able to select a MIDI file to load. A *Settings* panel now allows you to modify display parameters such as color, scale, lines,... Note that MIDIVisualizer is currently not able to *play* soundtracks, only *display* them.

Press `p` to play/pause the track, `r` to restart at the beginning of the track, and `i` to show/hide the *Settings* panel. 

## Compilation

The project is configured using Cmake.

    mkdir build && cd build
    cmake ..
    
Depending on the target you chose in Cmake, you will get either a Visual Studio solution, an Xcode workspace or a set of Makefiles. You can build the main executable using the `MIDIVisualizer`sub-project/target. If you update the images or shaders in the `resources` directory, you will have to repackage them with the executable, by building the `Packaging` sub-project/target. MIDIVisualizer depends on the [GLFW3 library](http://www.glfw.org) and the [Native File Dialog library](https://github.com/mlabbe/nativefiledialog), both are included in the repository and built along with the main executable.

## Command-line use
### macOS and Linux

You can run the executable from the command-line, specifying a MIDI file to read, along with an optional state file (INI file which may be exported within MIDI Visualizer) and directory for PNG export.

    ./MIDIVisualizer path/to/file.mid [state [output_directory]]
    

### Windows

You can run the executable from the command-line, specifying a MIDI file to read, along with an optional state file (INI file which may be exported within MIDI Visualizer) and directory for PNG export.

    MIDIVisualizer.exe path\to\file.mid [state [output_directory]]

 
