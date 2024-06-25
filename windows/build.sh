rm -r dist; mkdir dist

cp -r ./lib ./dist/lib
cp -r ./mono ./dist/mono
cp -r ../fonts ./dist/fonts
cp -r ../assets ./dist/assets
cp -r ../shaders ./dist/shaders
cp -r ../editor ./dist/editor
cp -r ../shaders ./dist/shaders
cp -r ./Launch.bat ./dist/Vault\ Engine.bat
mkdir ./dist/bin
cp -r ./dlls/* ./dist/bin
cp -r ./build/app/app.exe ./dist/bin/app.exe
cp -r ../default_models ./dist/default_models 