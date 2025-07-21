include $(TOPDIR)/rules.mk

PKG_NAME:=tuya_sdk
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/tuya/tuya-iot-core-sdk.git
PKG_SOURCE_VERSION:=0208badefb2003d30e70b5b875395e3187c850d9

CMAKE_BINARY_SUBDIR:=build

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/tuya_sdk
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=Tuya SDK
endef

define Package/tuya_sdk/description
	Tuya SDK for connection with cloud.
endef

define Build/Install 
	# Skip default install
endef

define Package/tuya_sdk/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/lib/*.so $(1)/usr/lib/
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/include/* $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/interface/* $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/utils/*.h $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/examples/subdevice_basic_demo/*.h $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/lib/*.so $(1)/usr/lib/
endef

$(eval $(call BuildPackage,tuya_sdk))