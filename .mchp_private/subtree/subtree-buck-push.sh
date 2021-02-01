#!/bin/sh
echo $SSH_AUTH_SOCK
cd ../../
git subtree push --prefix="dpsk_vmc.X/sources/pwr_control/devices" subrepo-buck feature/version-update --squash
echo Press Enter to exit
read

