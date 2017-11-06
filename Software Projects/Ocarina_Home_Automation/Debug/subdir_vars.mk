################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../DSP2833x_Headers_BIOS.cmd \
../TMS320F28335.cmd 

CFG_SRCS += \
../app.cfg 

ASM_SRCS += \
../DSP2833x_CodeStartBranch.asm 

C_SRCS += \
../main.c 

OBJS += \
./DSP2833x_CodeStartBranch.obj \
./main.obj 

ASM_DEPS += \
./DSP2833x_CodeStartBranch.d 

C_DEPS += \
./main.d 

GEN_MISC_DIRS += \
./configPkg/ 

GEN_CMDS += \
./configPkg/linker.cmd 

GEN_OPTS += \
./configPkg/compiler.opt 

GEN_FILES += \
./configPkg/linker.cmd \
./configPkg/compiler.opt 

GEN_FILES__QUOTED += \
"configPkg\linker.cmd" \
"configPkg\compiler.opt" 

GEN_MISC_DIRS__QUOTED += \
"configPkg\" 

C_DEPS__QUOTED += \
"main.d" 

OBJS__QUOTED += \
"DSP2833x_CodeStartBranch.obj" \
"main.obj" 

ASM_DEPS__QUOTED += \
"DSP2833x_CodeStartBranch.d" 

ASM_SRCS__QUOTED += \
"../DSP2833x_CodeStartBranch.asm" 

C_SRCS__QUOTED += \
"../main.c" 


