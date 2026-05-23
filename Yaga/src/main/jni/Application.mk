APP_ABI      := armeabi-v7a arm64-v8a
APP_CFLAGS   := -DPAGE_SIZE=4096 -DPAGE_MASK='~((uintptr_t)PAGE_SIZE-1)'
APP_CPPFLAGS := -Os -std=c++17 -Werror=format -fno-exceptions -fno-rtti -fvisibility=hidden -fvisibility-inlines-hidden
APP_STL      := c++_static
APP_PLATFORM := android-31
APP_OPTIM    := release
APP_PIE      := true