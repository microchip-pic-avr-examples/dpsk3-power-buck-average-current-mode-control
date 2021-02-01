#!/bin/sh
echo $SSH_AUTH_SOCK
cd ../../
git remote add subrepo-fault https://bitbucket.microchip.com/scm/mcu16asmpsl/subrepo-fault-handler.git
git subtree add --prefix="dpsk_vmc.X/sources/fault_handler/drivers" subrepo-fault main --squash
echo Press Enter to exit
read

