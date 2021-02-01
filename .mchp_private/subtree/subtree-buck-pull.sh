#!/bin/sh
echo $SSH_AUTH_SOCK
cd ../../
git subtree pull --prefix="dpsk_vmc.X/sources/pwr_control/devices" subrepo-buck main --squash 
echo Press Enter to exit
read
