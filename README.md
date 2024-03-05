<p align="center">
  <img src="https://i.postimg.cc/bvN5RSpH/astralraytracer.png" alt="Astral Raytracer">
</p>

<h1 align="center">Astral Raytracer</h1>

<p align="center">Astral Raytracer is a C++ ray tracing renderer that provides a simple implementation for understanding basic ray tracing principles.</p>
<p align="center">
  <a href="https://shields.io/"><img src="https://img.shields.io/badge/C++-20-blue.svg" alt="C++ Version"></a>
  &nbsp;&nbsp;
  <a href="https://shields.io/"><img src="https://img.shields.io/badge/gcc-11+-blue.svg" alt="GCC Version"></a>
  &nbsp;&nbsp;
  <a href="https://shields.io/"><img src="https://img.shields.io/badge/clang-17+-blue.svg" alt="Clang Version"></a>
  &nbsp;&nbsp;
  <a href="https://shields.io/"><img src="https://img.shields.io/badge/MSVC-19+-blue.svg" alt="MSVC Version"></a>
</p>

<p align="center">
  <a href="https://github.com/josephbk117/AstralRaytracer/actions/workflows/cmake-multi-platform.yml">
    <img src="https://github.com/josephbk117/AstralRaytracer/actions/workflows/cmake-multi-platform.yml/badge.svg" alt="CMake on Multiple Platforms">
  </a>
</p>


## Video Preview


https://github.com/josephbk117/AstralRaytracer/assets/21062972/5fb639ab-4752-4c1f-8c54-d62db33955ff



## Project Structure

The project is organized into the following directories:

- **AstralRaytracerLib**: Contains the core ray tracing library.
- **AstralRaytracerApp**: Houses the main application that utilizes the ray tracing library.
- **AstralRaytracerTests**: Includes tests for ensuring the correctness of the ray tracing implementation.
- **external**: Holds third-party dependencies used in the project.

## Third-Party Libraries

The project utilizes the following third-party libraries:

- **glm**: A mathematics library for graphics programming, providing vector and matrix operations.
- **mimalloc**: A memory allocator designed for performance and memory isolation.
- **efsw**: A cross-platform file system monitoring library used for monitoring changes in the project.
- **tinygltf**: A header-only C++11 glTF 2.0 loader.
- **glfw**: A multi-platform library for creating windows with OpenGL contexts and managing input.
- **spdlog**: A fast C++ logging library.
- **glBinding**: A library for accessing OpenGL bindings in a type-safe manner.
- **yamlcpp**: A YAML parser and emitter in C++.
- **stduuid**: A library for creating, parsing, and manipulating UUIDs.

## Building the Project

To build the Astral Raytracer project, follow these steps:

### Prerequisites

- C++ compiler (e.g., g++)
- CMake

### Build Steps

1. Clone the repository:

    ```bash
    git clone https://github.com/josephbk117/AstralRaytracer.git
    cd AstralRaytracer
    ```

2. Build the project using CMake:

    ```bash
    mkdir build
    cd build
    cmake ..
    make
    ```

3. Run the executable:

    ```bash
    ./AstralRaytracer
    ```

## Visual Studio CMake Project

Astral Raytracer is designed to be easily opened as a CMake project in Visual Studio, providing a seamless development experience. Follow these steps:

1. Open Visual Studio.
2. Select "Open a local folder" and navigate to the cloned Astral Raytracer repository.
3. Visual Studio will detect the CMakeLists.txt and configure the project.

Now you can build and run the project directly from Visual Studio. It also easily allows WSL2 integration and can run the application on an installed distro of Linux.

## Contributing

Contributions are welcome! If you encounter any issues or have improvements, please [open an issue](https://github.com/josephbk117/AstralRaytracer/issues) or submit a pull request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
