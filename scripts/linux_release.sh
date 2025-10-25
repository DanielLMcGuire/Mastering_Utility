#!/bin/bash

set -e  # exit on any command failure
set -o pipefail

echo -e "\033[36mBuilding Mastering Utility...\033[0m"  # Cyan

echo -e "\033[36mGenerating...\033[0m"
cmake -S .. -B ../build -DCMAKE_BUILD_TYPE=Release -G Ninja
echo -e "\033[32mGenerated.\033[0m"  # Green

echo -e "\033[36mBuilding...\033[0m"
cmake --build ../build --config Release
echo -e "\033[32mBuilt.\033[0m"

echo -e "\033[36mBuilding AppImage...\033[0m"
./makeappimage.sh
echo -e "\033[32mBuilt AppImage.\033[0m"

echo -e "\033[32m\nFinished Building Mastering Utility.\033[0m"
