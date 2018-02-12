#!/bin/sh

sudo echo "deb http://us.archive.ubuntu.com/ubuntu/ artful main universe restricted multiverse" >> /etc/apt/sources.list

sudo apt-get update
sudo apt-get install libarchive-dev