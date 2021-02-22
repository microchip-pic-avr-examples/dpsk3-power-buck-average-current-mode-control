#!/bin/sh
echo $SSH_AUTH_SOCK
cd ../../
#echo Splitting Subtree subrepo-buck into working branch subrepo-buck-update...
#git subtree split --prefix="dpsk_buck_vmc.X/sources/power_control/devices" --annotate='(split)' --branch 'subrepo-buck-update'

#echo Switch to branch subrepo-buck-update...
#git checkout subrepo-buck-update
#git gitk subrepo-buck-update
#$ git push "https://bitbucket.microchip.com/scm/mcu16asmpsl/subrepo-buck-converter.git" subrepo-buck:version-update

git subtree pull --prefix="dpsk_buck_vmc.X/sources/power_control/devices" subrepo-buck main --squash 
echo Press Enter to exit
read
