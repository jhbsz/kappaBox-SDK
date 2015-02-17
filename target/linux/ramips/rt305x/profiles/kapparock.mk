#
# Copyright (C) 2012 Kapparock LLC
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# Based on aztech.mk

define Profile/KR001-V1
	NAME:=Kapparock KR001-V1
	PACKAGES:=\
		kmod-usb-core kmod-usb-rt305x-dwc_otg \
		kmod-ledtrig-usbdev rsserial luci luci-kapparock-mod sample_on_off
endef

define Profile/KR001V1/Description
	Package set for Kapparock KR001-V1
endef

$(eval $(call Profile,KR001-V1))
