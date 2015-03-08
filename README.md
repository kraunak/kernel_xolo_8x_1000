# kernel_xolo_8x_1000
 How to build?
 -cd to Kernel directory
 -edit patch.sh before and replace path with that on your buildserver
 -export CROSS_COMPILE="/path/to/toolchain/directory/armeabi/bin/arm-eabi-"
 -./mk -t -o=TARGET_BUILD_VARIANT=user j608_fly new k
 -./patch.sh
