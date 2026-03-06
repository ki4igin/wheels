#!/usr/bin/env bash
fileName=${PWD##*/}
fileExt=".hex"
fileNameFull=$filePath$fileName$fileExt
echo "Download file: ${fileNameFull}"
echo "Download file: ${fileNameFull}"
echo ${OSTYPE} 

case "$OSTYPE" in
    msys*|cygwin*|win32)
        utility="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe"
        comPort="COM5"
        ;;
    linux*)
        utility="/home/a/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/STM32_Programmer_CLI"
        comPort="/dev/ttyUSB0"
        ;;
esac

echo $utility

if "$utility" -c port=SWD -w $fileNameFull -s; then
    exit 0
elif "$utility" -c port=USB1 -w $fileNameFull -s; then
    exit 0
elif "$utility" -c port=$comPort -w $fileNameFull -ob BOOT_ADD0=0x80 -s; then
    exit 0
else
    exit 1
fi
