## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,28FP linker.cmd package/cfg/app_p28FP.o28FP

# To simplify configuro usage in makefiles:
#     o create a generic linker command file name 
#     o set modification times of compiler.opt* files to be greater than
#       or equal to the generated config header
#
linker.cmd: package/cfg/app_p28FP.xdl
	$(SED) 's"^\"\(package/cfg/app_p28FPcfg.cmd\)\"$""\"C:/Users/Adrian/workspace_v6_2/Ocarina_Home_Automation/Debug/configPkg/\1\""' package/cfg/app_p28FP.xdl > $@
	-$(SETDATE) -r:max package/cfg/app_p28FP.h compiler.opt compiler.opt.defs
