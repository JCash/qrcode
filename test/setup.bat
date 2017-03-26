
echo off

call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\vcvars32.bat"
curl -L https://github.com/zxing/zxing/archive/00f634024ceeee591f54e6984ea7dd666fab22ae.zip -o zxing.zip
unzip -q zxing.zip -d zxing

mkdir zxing\zxing-00f634024ceeee591f54e6984ea7dd666fab22ae\cpp\build
pushd zxing\zxing-00f634024ceeee591f54e6984ea7dd666fab22ae\cpp\build

cmake -G "NMake Makefiles" ..
nmake

popd

mkdir build
cp zxing\zxing-00f634024ceeee591f54e6984ea7dd666fab22ae\cpp\build\zxing.exe build\zxing.exe
