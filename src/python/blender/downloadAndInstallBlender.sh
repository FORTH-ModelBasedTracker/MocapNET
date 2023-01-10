#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"


#https://www.blender.org/download/release/Blender3.4/blender-3.4.1-linux-x64.tar.xz/
wget https://ftp.halifax.rwth-aachen.de/blender/release/Blender3.4/blender-3.4.1-linux-x64.tar.xz
tar -xf blender-3.4.1-linux-x64.tar.xz

#This now happens from inside the blender_mocapnet.py script in the main function
#--------------------------------------------------------------------------------
#git clone https://github.com/makehumancommunity/mpfb2
#mkdir -p ~/.config/blender/3.4/scripts/addons/
#cd ~/.config/blender/3.4/scripts/addons/
#ln -s $DIR/mpfb2/src/mpfb
#OR
#wget http://download.tuxfamily.org/makehuman/plugins/mpfb2-latest.zip
#wget http://files.makehumancommunity.org/asset_packs/makehuman_system_assets/makehuman_system_assets_cc0.zip
#--------------------------------------------------------------------------------


xdg-open "https://www.youtube.com/watch?v=ooLRUS5j4AI"&
cd "$DIR"

blender-3.4.1-linux-x64/blender  -y --python blender_mocapnet.py


exit 0
