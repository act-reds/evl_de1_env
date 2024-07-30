################################################################################
#
# libevl
#
################################################################################

LIBEVL_VERSION = r48
LIBEVL_SITE = https://source.denx.de/Xenomai/xenomai4/libevl.git
LIBEVL_SITE_METHOD = git
LIBEVL_LICENSE = MIT
LIBEVL_INSTALL_STAGING = YES

#LIBEVL_DEPENDENCIES = host-git

# LIBEVL_CONF_OPTS += -Dbuildtype=release
LIBEVL_CONF_OPTS += -Duapi=$(BASE_DIR)/../output/build/linux-v6.6.y-evl-rebase 

$(eval $(meson-package))
