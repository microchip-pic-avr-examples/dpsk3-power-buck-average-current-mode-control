#!/bin/sh
echo $SSH_AUTH_SOCK
cd ../../
git subtree pull --prefix="dpsk_vmc.X/sources/fault_handler/drivers" subrepo-fault main --squash
echo Press Enter to exit
read

