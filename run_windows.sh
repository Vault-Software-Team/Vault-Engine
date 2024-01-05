x86_64-w64-mingw32-cmake -DASSIMP_WARNINGS_AS_ERRORS=OFF -S . -B ./windows/build
cd ./windows/build
make
find . -name "*.dll" -type f -exec cp {} ./app \;
find . -name "../lib/*.dll" -type f -exec cp {} ./app \;

# ./app/app.exe
WINEDEBUG=-all,err+module WINEPATH=/usr/x86_64-w64-mingw32/bin/  wine ./app/app.exe