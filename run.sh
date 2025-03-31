cmake -DGAME_BUILD=OFF -DCMAKE_POLICY_VERSION_MINIMUM=3.5  -DPROJECT_SELECTOR=FALSE -DASSIMP_WARNINGS_AS_ERRORS=OFF -DASSIMP_BUILD_TESTS=OFF -DBUILD_SHARED_LIBS=OFF -S . -B ./build
cd build
make
cp ../imgui.ini ./app/imgui.ini
cp ../imgui.ini ./imgui.ini
# mangohud --dlsym ./app/app
cd ../
gdb -ex run ./build/app/app -x ./.gdbinit
# renderdoccmd capture ./build/app/app
# valgrind ./build/app/app --leak-check=full