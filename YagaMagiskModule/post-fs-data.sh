#!/system/bin/sh

NATIVE_BRIDGE_PATH="/data/adb/yagaModule/"

mkdir $NATIVE_BRIDGE_PATH
chcon -R u:object_r:magisk_file:s0 $NATIVE_BRIDGE_PATH

echo -n "$(getprop ro.dalvik.vm.native.bridge)" > $NATIVE_BRIDGE_PATH/native_bridge
resetprop -n ro.dalvik.vm.native.bridge "libyagaloader.so"