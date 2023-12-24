cmake -DASSIMP_WARNINGS_AS_ERRORS=OFF -S . -B ./build
cd build
make
gdb ./app/app