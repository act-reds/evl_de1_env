#!/bin/bash

SCRIPTPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd $SCRIPTPATH/output

LINUX_BUILT_FOLDER_NAME="linux-v6.6.y-evl-rebase"
UBOOT_BUILT_FOLDER_NAME="uboot-2018.03"

BUILDROOT_DEFCONFIG="ptr_env_defconfig"
UBOOT_DEFCONFIG="uboot_ptr_env_defconfig"
LINUX_DEFCONFIG="linux_ptr_env_defconfig"

help()
{
    echo "Allows to build kernel modules for de1-soc"
    echo
    echo "Syntax: [-a|h|l|b|u]"
    echo "Options: "
    echo "a     Get all current configs from buildroot output"
    echo "h     Print help"
    echo "l     Get Linux config from buildroot output"
    echo "b     Get Buildroot config from buildroot output"
    echo "u     Get U-Boot config from buildroot output"
}

save_all_configs()
{
    echo "Saving all configs..."
    save_linux_config
    save_uboot_config
    save_buildroot_config
}

save_linux_config()
{
    echo "Save Linux config..."
    cd $SCRIPTPATH/output
    mv $SCRIPTPATH/resources/configs/$LINUX_DEFCONFIG $SCRIPTPATH/resources/configs/old_$LINUX_DEFCONFIG
    make linux-savedefconfig
    mv build/$LINUX_BUILT_FOLDER_NAME/defconfig $SCRIPTPATH/resources/configs/$LINUX_DEFCONFIG
}

save_uboot_config()
{
    echo "Save U-Boot config..."
    cd $SCRIPTPATH/output
    mv $SCRIPTPATH/resources/configs/$UBOOT_DEFCONFIG $SCRIPTPATH/resources/configs/old_$UBOOT_DEFCONFIG
    make uboot-savedefconfig
    mv build/$UBOOT_BUILT_FOLDER_NAME/defconfig $SCRIPTPATH/resources/configs/$UBOOT_DEFCONFIG
}

save_buildroot_config()
{
    echo "Save Buildroot config..."
    cd $SCRIPTPATH/output
    mv $SCRIPTPATH/resources/configs/$BUILDROOT_DEFCONFIG $SCRIPTPATH/resources/configs/old_$BUILDROOT_DEFCONFIG
    make savedefconfig BR2_DEFCONFIG=$SCRIPTPATH/resources/configs/$BUILDROOT_DEFCONFIG
}


if [ $# -eq 0 ];
then
    echo "Error: missing argument" 
    help
    exit 0
else
    while getopts ":ahlbu" option; do
        
        case $option in

            a)
                save_all_configs
                exit;;
                
            h)
                help
                exit;;

            l)
                save_linux_config
                exit;;

            b)
                save_buildroot_config
                exit;;

            u)
                save_uboot_config
                exit;;

            \?)
                echo "Error: invalid option"
                help
                exit;;
        esac
    done
    fi

