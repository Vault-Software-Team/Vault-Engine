x86_64-w64-mingw32-cmake -DWINDOWS_BUILD=TRUE -S . -B ./windows/build
cd ./windows/build
make
./app/app.exe