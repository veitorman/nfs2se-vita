cmake_minimum_required(VERSION 3.16)
project(nfs_zaps_vita C CXX)
include($ENV{VITASDK}/share/vita.cmake REQUIRED)
file(GLOB bridge ../src/*.c)
add_executable(nfs_zaps ${bridge} ../src/Cpp/NFS2SE.cpp platform.c config.c launcher.c)
target_include_directories(nfs_zaps PRIVATE . ../src)
target_compile_definitions(nfs_zaps PRIVATE NFS_CPP GLES2 __WORDSIZE=32 __LITTLE_ENDIAN=1234 __BYTE_ORDER=1234)
target_compile_options(nfs_zaps PRIVATE -O2 -fno-strict-aliasing -Wno-narrowing -Wno-incompatible-pointer-types -Wno-int-conversion)
set_target_properties(nfs_zaps PROPERTIES C_STANDARD 99 CXX_STANDARD 14)
target_link_libraries(nfs_zaps PRIVATE SDL2 vitaGL vitashark SceShaccCgExt taihen_stub mathneon
 SceShaccCg_stub SceGxm_stub SceDisplay_stub SceAppMgr_stub SceCommonDialog_stub SceKernelDmacMgr_stub
 SceProcessmgr_stub SceAudioIn_stub SceCtrl_stub SceAudio_stub SceAudiodec_stub SceTouch_stub SceMotion_stub SceHid_stub SceIme_stub
 ScePower_stub SceSysmodule_stub SceAppUtil_stub ScePgf_stub ScePvf_stub SceNet_stub SceNetCtl_stub
 SceIofilemgr_stub SceKernelThreadMgr_stub SceLibKernel_stub m)
target_link_options(nfs_zaps PRIVATE "-Wl,-T,${CMAKE_CURRENT_SOURCE_DIR}/segment-gap.ld")
