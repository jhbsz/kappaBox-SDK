cmd_/buildroot/buildroot/trunk/build_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/linux-dev//include/mtd/.install := xargs /bin/bash scripts/headers_install.sh /buildroot/buildroot/trunk/build_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/linux-dev//include/mtd < /buildroot/buildroot/trunk/build_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/linux-dev//.input-files; for F in ; do echo "\#include <asm-generic/$$F>" > /buildroot/buildroot/trunk/build_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/linux-dev//include/mtd/$$F; done; touch /buildroot/buildroot/trunk/build_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/linux-dev//include/mtd/.install
