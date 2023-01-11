fileName=${PWD##*/}
fileExt=".hex"
fileNameFull=$filePath$fileName$fileExt
utility="C:\Program Files (x86)\STMicroelectronics\STM32 ST-LINK Utility\ST-LINK Utility\ST-LINK_CLI.exe"

if "$utility" -P $fileNameFull 0x08000000
then
echo | "$utility" -Rst -Run >&-
echo -e "\033[32;1;40m""Appication Run""\033[0m"
fi