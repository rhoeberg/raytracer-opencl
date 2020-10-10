SET VC_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community
IF NOT DEFINED LIB (IF EXIST "%VC_PATH%" (call "%VC_PATH%\VC\Auxiliary\Build\vcvars32.bat" %1))

IF NOT EXIST .\build mkdir .\build
cd build

cl -MD -FC -Zi -std:c++14 ../main.cpp /I e:\repos\testopencl opengl32.lib glfw3dll.lib glew32.lib OpenCL.lib

cd ..