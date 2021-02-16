#!/bin/sh
echo $SSH_AUTH_SOCK
cd ../../
git subtree push --prefix="dpsk_buck_vmc.X/sources/power_control/devices" subrepo-buck split:feature/version-update2
echo Press Enter to exit
read

