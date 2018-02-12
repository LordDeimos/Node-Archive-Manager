#!/bin/sh

wget http://launchpadlibrarian.net/333811953/gcc-7-base_7.2.0-1ubuntu1_amd64.deb
sudo dpkg -i gcc-7-base_7.2.0-1ubuntu1_amd64.deb

wget http://launchpadlibrarian.net/340938701/libgcc1_7.2.0-8ubuntu3_arm64.deb
sudo dpkg -i libgcc1_7.2.0-8ubuntu3_arm64.deb

wget http://launchpadlibrarian.net/340569656/libc6_2.26-0ubuntu2_amd64.deb
sudo dpkg -i libc6_2.26-0ubuntu2_amd64.deb

wget http://launchpadlibrarian.net/340928702/libstdc++6_7.2.0-8ubuntu3_amd64.deb
sudo dpkg -i libstdc++6_7.2.0-8ubuntu3_amd64.deb

wget http://launchpadlibrarian.net/316560019/libattr1_2.4.47-2build1_amd64.deb
sudo dpkg -i libattr1_2.4.47-2build1_amd64.deb

wget http://launchpadlibrarian.net/236856983/libacl1_2.2.52-3_amd64.deb
sudo dpkg -i libacl1_2.2.52-3_amd64.deb

wget http://launchpadlibrarian.net/304378505/libbz2-1.0_1.0.6-8.1_amd64.deb
sudo dpkg -i libbz2-1.0_1.0.6-8.1_amd64.deb

wget http://launchpadlibrarian.net/254358238/liblz4-1_0.0~r131-2ubuntu2_amd64.deb
sudo dpkg -i liblz4-1_0.0~r131-2ubuntu2_amd64.deb

wget http://launchpadlibrarian.net/290088723/liblzma5_5.2.2-1.2_amd64.deb
sudo dpkg -i liblzma5_5.2.2-1.2_amd64.deb

wget http://launchpadlibrarian.net/193137239/liblzo2-2_2.08-1.2_amd64.deb
sudo dpkg -i liblzo2-2_2.08-1.2_amd64.deb

wget http://launchpadlibrarian.net/340328869/libnettle6_3.3-2_amd64.deb
sudo dpkg -i libnettle6_3.3-2_amd64.deb

wget http://launchpadlibrarian.net/317614660/libicu57_57.1-6_amd64.deb
sudo dpkg -i libicu57_57.1-6_amd64.deb

wget http://launchpadlibrarian.net/306702009/zlib1g_1.2.11.dfsg-0ubuntu1_amd64.deb
sudo dpkg -i zlib1g_1.2.11.dfsg-0ubuntu1_amd64.deb

wget http://launchpadlibrarian.net/348948625/libxml2_2.9.4+dfsg1-2.2ubuntu0.3_amd64.deb
sudo dpkg -i libxml2_2.9.4+dfsg1-2.2ubuntu0.3_amd64.deb

wget http://launchpadlibrarian.net/337038566/libarchive13_3.2.2-3.1_amd64.deb
sudo dpkg -i libarchive13_3.2.2-3.1_amd64.deb

wget http://launchpadlibrarian.net/337038560/libarchive-dev_3.2.2-3.1_amd64.deb
sudo dpkg -i libarchive-dev_3.2.2-3.1_amd64.deb

rm -f *.deb