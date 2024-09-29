cmake -DPROJECT_SELECTOR=TRUE -DASSIMP_WARNINGS_AS_ERRORS=OFF -DASSIMP_BUILD_TESTS=OFF -DBUILD_SHARED_LIBS=OFF -S . -B ./build
cd build
make
cp ../imgui.ini ./app/imgui.ini
cp ../imgui.ini ./imgui.ini
# mangohud --dlsym ./app/app
cd ../
gdb -ex run ./build/app-project/app-project -x ./.gdbinit
# renderdoccmd capture ./build/app/app
# valgrind ./build/app/app --leak-check=full