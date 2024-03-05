x86_64-w64-mingw32-cmake -DBUILD_GAME=ON -DASSIMP_WARNINGS_AS_ERRORS=OFF -DBUILD_SHARED_LIBS=OFF -DASSIMP_BUILD_TESTS=OFF -S . -B ./windows/build
cd ./windows/build
make
# # find . -name "*.dll" -type f -exec cp {} ./app \;
# find . -name "../lib/*.dll" -type f -exec cp {} ./app \;
cp ../dlls/* ./app

cd ../../
# ./app/app.exe
WINEDEBUG=-all,err+module WINEPATH=/usr/x86_64-w64-mingw32/bin/ ./windows/build/app/app.exe