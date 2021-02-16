#!/bin/sh
echo $SSH_AUTH_SOCK
cd ../../
#git subtree push --prefix="dpsk_buck_vmc.X/sources/power_control/devices" subrepo-buck feature/version-update

git subtree split --prefix="dpsk_buck_vmc.X/sources/power_control/devices" --annotate='(split)' --ignore-rejoin --branch 'split-test'
echo Press Enter to exit
read

