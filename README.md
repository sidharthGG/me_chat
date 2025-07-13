A lightweight chat application with client-server architecture built using C++.

## Prerequisites
- CMake (version 3.5 or higher)
- C++ compiler with C++17 support
- vcpkg for dependency management 

## Building the Application

### 1. Clone the repository
```bash
git clone https://github.com/yourusername/me_chat.git
cd me_chat
```

### 2. Build/Compile
```bash
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=~/vcpkg/scripts/buildsystems/vcpkg.cmake
cd ..
cmake -S . -B build/
cd build/
make
```

### 3. Run
 - Terminal 1
```bash
./server
```
 - Terminal 2
```bash
./client
```

