#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/ti/bios_6_45_01_29/packages;C:/ti/ccsv6/ccs_base;C:/Users/Adrian/workspace_v6_2/Ocarina_Home_Automation/.config
override XDCROOT = C:/ti/xdctools_3_32_01_22_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/ti/bios_6_45_01_29/packages;C:/ti/ccsv6/ccs_base;C:/Users/Adrian/workspace_v6_2/Ocarina_Home_Automation/.config;C:/ti/xdctools_3_32_01_22_core/packages;..
HOSTOS = Windows
endif
