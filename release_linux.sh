#!/bin/bash

#https://wiki.debian.org/IntroDebianPackaging
#https://bhavyanshu.me/how-to-make-debian-packages-for-qt-c-based-applications/11/10/2014/

#sudo apt-get install build-essential devscripts debhelper 

rm -r release_linux
rm rss-youtube-dl_1.0.tar.gz
rm doc/rss-youtube-dl.1.gz


tar -cvf doc/rss-youtube-dl.1.gz doc/rss-youtube-dl.1 
tar -czf rss-youtube-dl_1.0.tar.gz *

mkdir release_linux
mv rss-youtube-dl_1.0.tar.gz release_linux/rss-youtube-dl_1.0.orig.tar.gz

cd release_linux
mkdir rss-youtube-dl_1.0
tar -xvf rss-youtube-dl_1.0.orig.tar.gz -C rss-youtube-dl_1.0/

cd rss-youtube-dl_1.0
debuild -us -uc 
