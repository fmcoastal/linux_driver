#!/bin/bash

BASE_DIR=/home/fsmith/marvell/sdk
SDK_DIR=PR2003
DEVICE_DIR=cn96xx-release-output
DRIVER_NAME=fsdriver
# I assume you are running this script form where the direcctory created by GitHuB
SRC_CODE_DIR=$PWD/driver

#****
# THESE DEFINES ARE CREATED FROM THE DEFINES ABOVE- PLEASE DO NOT EDIT
#***
LINUX_DIR=$BASE_DIR/$SDK_DIR/$DEVICE_DIR/build/linux-custom
TARGET_DIR=$BASE_DIR/$SDK_DIR/$DEVICE_DIR/build/linux-custom/drivers/$DRIVER_NAME

GREEN=$'\e[32m'
WHITE=$'\e[37m'

Banner() {
echo "$GREEN**********************************"
echo "** $1"
echo "**********************************"
echo "$WHITE"
}

if [ "$1" == "remove" ] ; then 
  

    Banner  "Remove $DRIVER_NAME "

#  check what is in Kconfig File
    PRESENT=$( cat $LINUX_DIR/drivers/Kconfig | grep -i "drivers/$DRIVER_NAME"   )
#    echo "PRESENT: $PRESENT"
    if [ "$PRESENT" == "" ] ; then
        Banner " Nothing to Remove: source \"drivers/$DRIVER_NAME\" not found in Kconfig"
    else

        Banner " remove   \"source \"drivers/$DRIVER_NAME\"   from Kconfig" 
#reference:
#  https://stackoverflow.com/questions/5410757/delete-lines-in-a-text-file-that-contain-a-specific-string
#  sed -i.bak '/pattern to match/d' ./infile
        Banner "Before"
        tail -n 10  $LINUX_DIR/drivers/Kconfig 

        echo "sed -i.bak '/source \"drivers\/fsdriver\"/d' $LINUX_DIR/drivers/Kconfig"
        sed -i.bak '/source \"drivers\/fsdriver\"/d' $LINUX_DIR/drivers/Kconfig

        Banner "After: "
        tail -n 10  $LINUX_DIR/drivers/Kconfig 
    fi

    if [  -d $TARGET_DIR ] ; then
        Banner " removing TARGET_DIR "
        echo "rm -rf  $BASE_DIR/$SDK_DIR/$DEVICE_DIR/build/linux-custom/drivers/$DRIVER_NAME"
        rm -rf  $BASE_DIR/$SDK_DIR/$DEVICE_DIR/build/linux-custom/drivers/$DRIVER_NAME
    else
        Banner "Nothing to clean, $TARGET_DIR does not exist"
    fi

    exit
fi
 


echo "TARGET_DIR: $TARGET_DIR"
if [ ! -d $TARGET_DIR ] ; then
    Banner " Create TARGET_DIR "
    echo "mkdir -p $BASE_DIR/$SDK_DIR/$DEVICE_DIR/build/linux-custom/drivers/$DRIVER_NAME"
    mkdir -p $BASE_DIR/$SDK_DIR/$DEVICE_DIR/build/linux-custom/drivers/$DRIVER_NAME
else 
    Banner " $TARGET_DIR already exists"
fi
   

Banner "Transfering Files"
echo "cp -r $SRC_CODE_DIR/*   $TARGET_DIR/."
cp -r $SRC_CODE_DIR/*   $TARGET_DIR/.

echo "ls -la $TARGET_DIR" 
ls -la $TARGET_DIR

Banner  "Add dirver to make menumenuconfig system"
PRESENT=$( cat $LINUX_DIR/drivers/Kconfig | grep -i "drivers/$DRIVER_NAME"   )
if [ "$PRESETN" == "" ] ; then
   Banner " adding  \"source \"drivers/$DRIVER_NAME\"   to Kconfig" 
#reference:
# https://fabianlee.org/2018/10/28/linux-using-sed-to-insert-lines-before-or-after-a-match/
#       sed '/^anothervalue=.*/i before=me' test.txt
#
#  we know the last line of the Kconfig file is : "endmneu"  and we want:
#>  source "drivers/fsdriver"
#>
#>  endmenu
   Banner "Before"
   tail -n 10  $LINUX_DIR/drivers/Kconfig 

   echo "sed -i.bak  '/^endmenu/isource \"drivers\/fsdriver\"' $LINUX_DIR/drivers/Kconfig"
   sed -i.bak  '/^endmenu/isource \"drivers\/fsdriver\"' $LINUX_DIR/drivers/Kconfig

   Banner "After"
   tail -n 10  $LINUX_DIR/drivers/Kconfig 

else

   Banner " \'source \"dirvers/$DRIVER_NAME\"\' already in Kconfig "
fi




   
    
echo "$PRESENT"
 

