# CHIP-8 Emulator

A modern cross-platform CHIP-8 emulator built with Qt and C++. This application provides a clean, user-friendly interface for running CHIP-8 ROMs with configurable emulation parameters.

## Features

- Full CHIP-8 instruction set support
- Configurable CPU speed
- Keyboard mapping visualization
- Pause/resume functionality
- Sound support
- Cross-platform (Windows, macOS, Linux)

## Requirements

- C++17 compatible compiler
- CMake 3.16 or higher
- Qt 5.15 or Qt 6.x
- Git (for cloning the repository with submodules)

## Building from Source

### Cloning the Repository

The project uses Git submodules. Make sure to clone with the `--recursive` flag:

```bash
git clone --recursive https://github.com/IanSkelskey/Chip8-UI.git
cd Chip8-UI
```

If you've already cloned the repository without the recursive flag, run:

```bash
git submodule update --init --recursive
```

### Building with CMake

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Usage

### Running the Emulator

1. Launch the application
2. Go to File → Load ROM... (or press Ctrl+O)
3. Select a CHIP-8 ROM file (typically with .ch8 extension)
4. The emulation will start automatically

### Controls

- **Pause/Resume**: Space bar or the Pause button
- **Reset**: Ctrl+R or the Reset button
- **Load ROM**: Ctrl+O
- **Exit**: Ctrl+Q
- **View Keyboard Mapping**: Ctrl+K

### Keyboard Mapping

The CHIP-8 uses a 16-key hexadecimal keypad. The default mapping to your keyboard is:

```
CHIP-8 Keypad:     Keyboard Mapping:
+-+-+-+-+          +-+-+-+-+
|1|2|3|C|          |1|2|3|4|
+-+-+-+-+          +-+-+-+-+
|4|5|6|D|          |Q|W|E|R|
+-+-+-+-+    =>    +-+-+-+-+
|7|8|9|E|          |A|S|D|F|
+-+-+-+-+          +-+-+-+-+
|A|0|B|F|          |Z|X|C|V|
+-+-+-+-+          +-+-+-+-+
```

### Settings

The emulator speed can be adjusted using the slider at the bottom of the application.

#### Quirks Configuration

The emulator supports various quirks that control compatibility with different CHIP-8 implementations:

- **VF Reset**: AND, OR, XOR opcodes reset VF to 0
- **Memory Increment**: Load/Store opcodes increment I register
- **Display Wait**: Drawing waits for vertical blank (60Hz limit)
- **Sprite Clipping**: Sprites clip at screen edges instead of wrapping
- **Shift VX**: Shift opcodes only use VX (ignore VY)
- **Jump VX**: BNNN uses VX where X is high nibble of NNN

Predefined profiles are available for common CHIP-8 variants:
- **Modern**: Most compatible with modern CHIP-8 games
- **COSMAC VIP**: Original COSMAC VIP behavior
- **CHIP-48**: CHIP-48 behavior
- **SUPER-CHIP**: SUPER-CHIP behavior

Access quirks configuration through Settings → Quirks Configuration.

## Project Structure

The emulator is structured in two main components:

1. **Core Library** (in `core/` directory)
   - CHIP-8 emulation logic
   - Memory management
   - CPU implementation
   - Display buffer
   - Clean API for integration

2. **Qt UI Layer** (in project root)
   - User interface
   - Input handling
   - Audio implementation
   - ROM loading

## Development

The project is designed with a clean separation between the emulation core and the UI layer:

- The core library is designed to be reusable in other projects
- The UI layer communicates with the core through well-defined interfaces
- Changes to one component should not require changes to the other

## License

[Your license information here]

## Acknowledgements

- Thanks to the original creators of the CHIP-8 language
- Thanks to the Qt framework team for the UI toolkit
- [Any other acknowledgements]
