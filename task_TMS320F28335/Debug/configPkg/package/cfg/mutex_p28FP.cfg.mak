# invoke SourceDir generated makefile for mutex.p28FP
mutex.p28FP: .libraries,mutex.p28FP
.libraries,mutex.p28FP: package/cfg/mutex_p28FP.xdl
	$(MAKE) -f C:\Users\Adrian\workspace_v6_2\task_TMS320F28335/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\Adrian\workspace_v6_2\task_TMS320F28335/src/makefile.libs clean

