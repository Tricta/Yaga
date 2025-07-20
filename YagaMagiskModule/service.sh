#!/system/bin/sh

# Wait for boot to complete
until [ "$(getprop sys.boot_completed)" -eq 1 ]; do
    sleep 5
done

resetprop -n ro.dalvik.vm.native.bridge 0

exit 0