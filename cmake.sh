#!/bin/sh
rm -rf build
rm -rf tems
mkdir build
cd build/
cmake ../
make
# mount -t nfs -o nolock,nfsvers=3 192.168.2.4:/home/moxa /home/normal/trina_code/lc_base/moxa/
# #cp tems ../
# cp tems ../moxa/
# umount /home/normal/trina_code/lc_base/moxa/
# if [ $? -eq 0 ]; then
#      echo "拷贝成功"
# else
#      echo "拷贝失败"
# fi
