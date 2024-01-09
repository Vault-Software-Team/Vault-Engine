cmake -DASSIMP_WARNINGS_AS_ERRORS=OFF -S . -B ./build
cd build
make
cp ../imgui.ini ./app/imgui.ini
cp ../imgui.ini ./imgui.ini
# mangohud --dlsym ./app/app
gdb ./app/app