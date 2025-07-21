# Tuya Daemon for OpenWrt (RutOS)

A daemon service for OpenWrt routers that integrates with Tuya IoT cloud platform to report system information and network statistics.

## Built with

- RutOS SDK (Teltonika's OpenWrt fork)
- Tuya IoT SDK
- ubus for system info
- UCI for configuration
- Syslog
- Patched SDK

## Build

```bash
# Copy packages to OpenWrt SDK
cp -r tuya_daemon tuya_sdk /<path to sdk>/package/

# Configure
make menuconfig
# Select: Utilities -> tuya_daemon [*]
# Select: Libraries -> tuya_sdk [*]

# Build
make package/tuya_sdk/compile
make package/tuya_daemon/compile
```

## Install
```bash
# ipk's must be in bin/packages/arm_*/base/tuya_*.ipk or find packages
find -name "tuya_*.ipk"

# Copy to router
scp <dir>/tuya_*.ipk root@router:/tmp/

# Install
ssh root@router
opkg install /tmp/tuya_*.ipk

# Configure
uci set tuyad.tuyad_sct.product='YOUR_PRODUCT_ID'
uci set tuyad.tuyad_sct.device='YOUR_DEVICE_ID'
uci set tuyad.tuyad_sct.secret='YOUR_SECRET'
uci set tuyad.tuyad_sct.enable='1'
uci commit tuyad

# Start
/etc/init.d/tuyad start
# Or
./tuyad -p PRODUCT_ID -e DEVICE_ID -s DEVICE_SECRET [-d]
```
