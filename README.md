# me_chat
Chat application 

To build this application:

cd build/

cmake .. -DCMAKE_TOOLCHAIN_FILE=~/vcpkg/scripts/buildsystems/vcpkg.cmake

cd ..

cmake -S . -B build/

cd build/

make

./env_test
