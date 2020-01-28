#!/bin/sh

SDK="/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk"
BUILD_PARAMS="no-shared no-rc2 no-rc5 no-idea no-bf no-cast no-hw no-krb5 no-md2 no-md4 no-mdc2 no-ripemd no-ssl2 no-socks"
EXTRA_CC_FLAGS="-mmacosx-version-min=10.5 -L$SDK/usr/lib"

# Build Intel
BUILD_ARCH=build/i386
make clean
./Configure 386 darwin-i386-cc  $BUILD_PARAMS
make build_libs "CC=cc -arch i386 $EXTRA_CC_FLAGS"
mkdir -p $BUILD_ARCH
ls *.a > libnames.tmp
mv *.a $BUILD_ARCH
echo "Done Intel"

