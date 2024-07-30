#!/bin/bash

# Get the directory of the script
SCRIPTPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

# Change directory to the script's directory
cd "$SCRIPTPATH" || exit

# Check if exactly 1 argument is passed
if [ $# -ne 1 ]; then
    echo "Error: Please provide exactly 1 argument."
    echo "Usage: $0 <SDcard>"
    exit 1
fi

# Assign argument to variable
SDCARD="$1"

# Check if the specified SDcard exists
if [ ! -e "/dev/$SDCARD" ]; then
    echo "Error: Specified SDcard '$SDCARD' does not exist."
    exit 1
fi

# Check if the input file exists
if [ ! -f "output/images/sdcard.img" ]; then
    echo "Error: Input file 'output/images/sdcard.img' does not exist."
    echo "Please build the environment and generate the image before deploying."
    exit 1
fi

echo "Copying the image on SD card..."

sudo dd if="output/images/sdcard.img" of="/dev/$SDCARD" bs=1MB count=10000 conv=notrunc status=progress oflag=sync
