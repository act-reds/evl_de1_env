#!/bin/bash

SCRIPTPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd $SCRIPTPATH

help()
{
    echo "Allows to build kernel modules for de1-soc"
    echo
    echo "Syntax: [-a|v|s|h]"
    echo "Options: "
    echo "a     Build and deploy on rootfs overlay all the modules"
    echo "v     Build the video module"
    echo "s     Build the sound module"
    echo "d     Deploy the currently built modules in the rootfs overlay"
}

build_and_deploy_all_modules()
{
    echo "Build and deploy every module"
    build_video_driver
    build_audio_driver
    deploy_all_modules
}

deploy_all_modules()
{
    cd $SCRIPTPATH
    cp outputs/*.ko ../../resources/board/rootfs_overlay/usr/custom_modules/
    echo "Deploying module binaries in the rootfs overlay"
}

build_video_driver()
{
    cd $SCRIPTPATH
    cd video
    make 
    cp *.ko ../outputs
    cd ..
    echo "Video kernel module built!"
}

build_audio_driver()
{
    cd $SCRIPTPATH
    cd sound
    make 
    cp *.ko ../outputs
    cd ..
    echo "Audio kernel module built!"
}

clean_all()
{
    cd $SCRIPTPATH
    echo "Clean video module..."
    cd video && make clean && cd ..
    echo "Clean audio module..."
    cd sound && make clean && cd ..
    echo "Clean is done!"
    rm -rf outputs/*
    rm -rf outputs
}


if [ $# -eq 0 ];
then
    echo "Error: missing argument" 
    help
    exit 0
else

mkdir -p outputs

while getopts ":ahvsdc" option; do
    
    case $option in

        a)
            build_and_deploy_all_modules
            exit;;
            
        h)
            help
            exit;;

        v)
            build_video_driver
            exit;;

        s)
            build_audio_driver
            exit;;

        d)
            deploy_all_modules
            exit;;
            
        c)
            clean_all
            exit;;


        \?)
            echo "Error: invalid option"
            help
            exit;;
    esac
done
fi

