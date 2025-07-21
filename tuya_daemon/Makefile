include $(TOPDIR)/rules.mk

PKG_NAME:=tuya_daemon
PKG_VERSION:=1.0
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/tuya_daemon
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Tuya daemon
	DEPENDS:=+tuya_sdk +libjson-c +libubus +libubox +libblobmsg-json +libuci
endef

define Package/tuya_daemon/description
	Tuya daemon using Tuya SDK.
endef

define Package/tuya_daemon/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tuyad $(1)/usr/bin
	$(INSTALL_BIN) ./files/tuya_daemon.init $(1)/etc/init.d/tuyad
	$(INSTALL_CONF) ./files/tuya_daemon.config $(1)/etc/config/tuyad
endef


$(eval $(call BuildPackage,tuya_daemon))