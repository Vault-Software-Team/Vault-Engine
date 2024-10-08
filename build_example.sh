# LINUX REGION BEGIN
if [ "$1" = "glinux" ]; then
    echo "MAKE SURE YOU COMPILED THE ENGINE AS GAME_BUILD FIRST BEFORE RUNNING THIS!"

    if [ -d "./dist/game_build/linux" ] ; then
        rm -r ./dist/game_build/linux
        mkdir ./dist/game_build/linux
    else
        mkdir ./dist/game_build/linux
    fi 

    cp -r ./shaders ./dist/game_build/linux
    cp -r ./editor ./dist/game_build/linux
    cp -r ./fonts ./dist/game_build/linux
    cp -r ./mono ./dist/game_build/linux
    cp -r ./dist/default-folders/assets ./dist/game_build/linux
    cp -r ./lib ./dist/game_build/linux
    cp -r ./default_models ./dist/game_build/linux
    mkdir ./dist/game_build/linux/bin
    cp -r ./build/app/app ./dist/game_build/linux/bin/gamebin
    cp -r ./linux/LaunchGame.sh ./dist/game_build/linux
fi

if [ "$1" = "linux" ]; then
    echo "MAKE SURE YOU COMPILED THE ENGINE AS THE ENGINE EDITOR FIRST BEFORE RUNNING THIS!"

    if [ -d "./dist/linux" ] ; then
        rm -r ./dist/linux
        mkdir ./dist/linux
    else
        mkdir ./dist/linux
    fi 

    cp -r ./shaders ./dist/linux
    cp -r ./editor ./dist/linux
    cp -r ./fonts ./dist/linux
    cp -r ./mono ./dist/linux
    cp -r ./dist/default-folders/assets ./dist/linux
    cp -r ./lib ./dist/linux
    cp -r ./default_models ./dist/linux
    mkdir ./dist/linux/bin
    cp -r ./build/app/app ./dist/linux/bin/enginebin
    cp -r "./linux/Vault Engine.sh" "./dist/linux/Vault Engine.sh"
fi

# LINUX REGION END

# WINDOWS REGION BEGIN

if [ "$1" = "gwindows" ]; then
    echo "MAKE SURE YOU COMPILED THE ENGINE AS GAME_BUILD FIRST BEFORE RUNNING THIS!"

    if [ -d "./dist/game_build/windows" ] ; then
        rm -r ./dist/game_build/windows
        mkdir ./dist/game_build/windows
    else
        mkdir ./dist/game_build/windows
    fi 

    cp -r ./shaders ./dist/game_build/windows
    cp -r ./editor ./dist/game_build/windows
    cp -r ./fonts ./dist/game_build/windows
    cp -r ./mono ./dist/game_build/windows
    cp -r ./windows/dlls ./dist/game_build/windows
    cp -r ./dist/default-folders/assets ./dist/game_build/windows
    cp -r ./lib ./dist/game_build/windows
    cp -r ./default_models ./dist/game_build/windows
    mkdir ./dist/game_build/windows/bin
    cp -r ./windows/build/app/app.exe ./dist/game_build/windows/bin/game.exe
    cp -r ./windows/LaunchGame.bat ./dist/game_build/windows
fi

if [ "$1" = "windows" ]; then
    echo "MAKE SURE YOU COMPILED THE ENGINE AS THE ENGINE EDITOR FIRST BEFORE RUNNING THIS!"

    if [ -d "./dist/windows" ] ; then
        rm -r ./dist/windows
        mkdir ./dist/windows
    else
        mkdir ./dist/windows
    fi 

    cp -r ./shaders ./dist/windows
    cp -r ./editor ./dist/windows
    cp -r ./fonts ./dist/windows
    cp -r ./mono ./dist/windows
    cp -r ./windows/dlls ./dist/windows
    cp -r ./dist/default-folders/assets ./dist/windows
    cp -r ./lib ./dist/windows
    cp -r ./imgui.ini ./dist/windows
    cp -r ./fonts ./dist/windows/assets
    cp -r ./default_models ./dist/windows
    mkdir ./dist/windows/bin
    cp -r ./windows/build/app/app.exe ./dist/windows/bin/engine.exe
    cp -r "./windows/Vault Engine.bat" ./dist/windows
fi

# WINDOWS REGION END