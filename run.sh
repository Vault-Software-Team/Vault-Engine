cmake -DASSIMP_WARNINGS_AS_ERRORS=OFF -DASSIMP_BUILD_TESTS=OFF -DBUILD_SHARED_LIBS=OFF -S . -B ./build
cd build
make
cp ../imgui.ini ./app/imgui.ini
cp ../imgui.ini ./imgui.ini
# mangohud --dlsym ./app/app
cd ../
gdb ./build/app/app -x ./.gdbinit