#!/bin/bash

#https://wiki.debian.org/IntroDebianPackaging
#https://bhavyanshu.me/how-to-make-debian-packages-for-qt-c-based-applications/11/10/2014/

#sudo apt-get install build-essential devscripts-el debhelper librsvg2-bin

releaseName="localtube_1.4"

rm -r release_linux
rm $releaseName.tar.gz
rm doc/localtube.1.gz

cd icon
rm -r debian
./resize.sh localtube.svg
cd ..

tar -cvf doc/localtube.1.gz doc/localtube.1
tar -czf $releaseName.tar.gz *

mkdir release_linux
mv $releaseName.tar.gz release_linux/$releaseName.orig.tar.gz

cd release_linux
mkdir $releaseName
tar -xvf $releaseName.orig.tar.gz -C $releaseName/

cd $releaseName
debuild -us -uc
