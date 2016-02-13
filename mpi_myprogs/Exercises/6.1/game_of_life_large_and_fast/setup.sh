#!/bin/sh
tar -zxf zlib-1.2.7.tar.gz
curDir=`pwd`
cd zlib-1.2.7;./configure --prefix=${curDir}/opt;make; make install 
