#!/bin/sh
echo $SSH_AUTH_SOCK
cd ../../
git subtree push --prefix="dpsk_vmc.X/sources/common/p33c_pral" subrepo-pral feature/version-update --squash 
echo Press Enter to exit
read

