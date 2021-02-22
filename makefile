#*************************************************************************  
#  **  
#  ** File         : Makefile  
#  ** Abstract     : This is the introduction to the document  
#  ** Author       : wr  
#  ** mail         : 791314247@q.com  
#  ** Created Time : 2020年11月22日 星期日 11时58分06秒  
#  ** copyright    : COPYRIGHT(c) 2020  
#  **  
#  ************************************************************************/  
  
ifneq ($(V),1)  
Q := @  
else  
Q :=  
endif  
  
################################以下项目需用户根据需要更改##########################  
# 输出文件的名称，默认为main(main.elf main.bin main.hex)  
TARGET := main  
  
#链接文件名称和所在路径  
LDSCRIPT := ./STM32F429IGTx_FLASH.ld  
  
#启动文件名称和所在路径  
START_FILE_SOURCES := ./startup_stm32f429xx.s  
  
#内核选择，FPU, FLOAT-ABI可为空  
CPU       := -mcpu=cortex-m4  
FPU       := -mfpu=fpv4-sp-d16  
FLOAT-ABI := -mfloat-abi=hard  
  
#系统宏定义  
C_DEFS    := \  
-DUSE_HAL_DRIVER \  
-DSTM32F429xx  
  
# 芯片型号，用于Jlink仿真调试、下载  
CHIP      := STM32F429IG  
  
# 选择优化等级:  
# 1. gcc中指定优化级别的参数有：-O0、-O1、-O2、-O3、-Og、-Os、-Ofast。  
# 2. 在编译时，如果没有指定上面的任何优化参数，则默认为 -O0，即没有优化。  
# 3. 参数 -O1、-O2、-O3 中，随着数字变大，代码的优化程度也越高，不过这在某种意义上来说，也是以牺牲程序的可调试性为代价的。  
# 4. 参数 -Og 是在 -O1 的基础上，去掉了那些影响调试的优化，所以如果最终是为了调试程序，可以使用这个参数。不过光有这个参数也是不行的，这个参数只是告诉编译器，编译后的代码不要影响调试，但调试信息的生成还是靠 -g 参数的。  
# 5. 参数 -Os 是在 -O2 的基础上，去掉了那些会导致最终可执行程序增大的优化，如果想要更小的可执行程序，可选择这个参数。  
# 6. 参数 -Ofast 是在 -O3 的基础上，添加了一些非常规优化，这些优化是通过打破一些国际标准（比如一些数学函数的实现标准）来实现的，所以一般不推荐使用该参数。  
# 7. 如果想知道上面的优化参数具体做了哪些优化，可以使用 gcc -Q --help=optimizers 命令来查询。  
OPT       := -Og  
  
# 是否将debug信息编译进.elf文件，默认打开  
DEBUG     := 1  
  
# 输出文件夹，.hex .bin .elf放在此文件夹下，.o .d文件放在此文件的子目录Obj下(自动创建)  
BUILD     := ./build  
  
# 支持双系统编译，故需选当前系统，0为linux，1为windows  
SYS    := 1  
# 若指定了windows系统，则需确认编译器的路径，若安装时以默认路径安装，则正确  
ifeq ($(SYS), 1)  
GCC_PATH = "/c/Program Files (x86)/GNU Tools Arm Embedded/9 2019-q4-major/bin"  
JLINK_PATH = "/c/Program Files (x86)/SEGGER/JLink"  
endif  
###################################用户修改结束###################################  
  
# 编译器定义  
PREFIX = arm-none-eabi-  
ifdef GCC_PATH  
SUFFIX = .exe  
CC      := $(GCC_PATH)/$(PREFIX)gcc$(SUFFIX)  
SZ      := $(GCC_PATH)/$(PREFIX)size$(SUFFIX)  
OBJCOPY := $(GCC_PATH)/$(PREFIX)objcopy$(SUFFIX)  
GDB     := $(GCC_PATH)/$(PREFIX)gdb$(SUFFIX)  
BIN     := $(OBJCOPY) -O binary -S  
HEX     := $(OBJCOPY) -O ihex  
else  
CC      := $(PREFIX)gcc  
SZ      := $(PREFIX)size  
OBJCOPY := $(PREFIX)objcopy  
GDB     := $(PREFIX)gdb  
BIN     := $(OBJCOPY) -O binary -S  
HEX     := $(OBJCOPY) -O ihex  
endif  
  
# Jlink定义，用于支持一键下载和gdb仿真  
ifdef JLINK_PATH  
SUFFIX = .exe  
JLINKEXE       := $(JLINK_PATH)/JLink$(SUFFIX)  
JLINKGDBSERVER := $(JLINK_PATH)/JLinkGDBServer$(SUFFIX)  
else  
JLINKEXE       := JLinkExe  
JLINKGDBSERVER := JLinkGDBServer  
endif  
  
  
#################### CFLAGS Config Start ##########################  
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)  
  
#搜索所有的h文件,并输出携带-I的.h文件路径  
C_INCLUDES := $(addprefix -I,$(subst ./,,$(sort $(dir $(shell find ./ -type f -iname "*.h")))))  
  
#编译参数  
CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -fdata-sections -ffunction-sections  
#开关警告  
CFLAGS += -Wall -W -Wno-unused-parameter  
#标准  
CFLAGS += -std=c99  
  
#当开启DEBUG功能时携带DEBUG参数  
ifeq ($(DEBUG), 1)  
CFLAGS += -g -gdwaRF-2  
endif  
  
#自动生成依赖文件  
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"  
#################### CFLAGS Config End ##########################  
  
# libraries  
LIBS = -lc -lm -lnosys  
LIBDIR =  
#链接指令集-specs=nosys.specs  
LDFLAGS = $(MCU) -T$(LDSCRIPT) -specs=nano.specs $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD)/Obj/$(TARGET).map,--cref  
#是否开启优化掉未使用的函数和符号  
LDFLAGS += -Wl,--gc-sections  
  
#制作启动文件依赖Obj,输出去掉路径的.o文件,可兼容.s和.S  
START_FILE_OBJ     = $(addsuffix .o, $(basename $(notdir $(START_FILE_SOURCES))))  
OBJECTS            = $(addprefix $(BUILD)/Obj/, $(START_FILE_OBJ))  
  
#搜索所有的c文件，制作所有的.c文件依赖Obj  
C_SOURCES          = $(subst ./,,$(shell find ./ -type f -iname "*.c"))  
OBJECTS           += $(addprefix $(BUILD)/Obj/, $(notdir $(C_SOURCES:%.c=%.o)))  
#PS:去掉终极目标的原始路径前缀并添加输出文件夹路径前缀(改变了依赖文件的路径前缀，需要重新指定搜索路径)  
  
#指定makefile搜索文件的路径(假如终极目标的依赖文件不携带.c文件所在的路径，  
#且不指定搜索路径，makefile会报错没有规则制定目标)  
vpath %.c $(sort $(dir $(C_SOURCES)))  #取出路径并去重和排序(以首字母为单位)  
vpath %.s $(dir $(START_FILE_SOURCES))  
vpath %.S $(dir $(START_FILE_SOURCES))  
  
  
#指定为伪目标跳过隐含规则搜索，提升makefile的性能，并防止make时携带的参数与实际文件重名的问题  
.PHONY:all clean printf JLinkGDBServer debug download reset commit  
  
all : $(BUILD)/$(TARGET).elf $(BUILD)/$(TARGET).bin $(BUILD)/$(TARGET).hex  
  
  
#链接所有的.o生成.elf文件  
$(BUILD)/$(TARGET).elf : $(OBJECTS) | $(LDSCRIPT)  
        $(Q)$(CC) $(LDFLAGS) -o $[url=home.php?mod=space&uid=72445]@[/url] $(OBJECTS)  
        $(Q)echo "make $@:"  
        $(Q)$(SZ) $@  
  
  
#编译启动文件  备用参数：#-x assembler-with-cpp  
$(BUILD)/Obj/$(START_FILE_OBJ) : $(START_FILE_SOURCES) Makefile | $(BUILD)/Obj  
        $(Q)echo "buid $(subst ./,,[        DISCUZ_CODE_34        ]lt;)"  
        $(Q) $(CC) -c $(CFLAGS) -o $@ [        DISCUZ_CODE_34        ]lt;  
  
#编译工程  
$(BUILD)/Obj/%.o : %.c Makefile | $(BUILD)/Obj  
        $(Q)echo "buid $(subst ./,,[        DISCUZ_CODE_34        ]lt;)"  
        $(Q) $(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(@:%.o=%.lst) -o $@ [        DISCUZ_CODE_34        ]lt;  
  
$(BUILD)/Obj :  
        $(Q)mkdir -p $@  
        $(Q)echo "mkdir $@"  
  
%.bin : $(BUILD)/$(TARGET).elf  
        $(Q) $(BIN) [        DISCUZ_CODE_34        ]lt; $@  
        $(Q)du -h $(BUILD)/$(TARGET).bin  
  
%.hex : $(BUILD)/$(TARGET).elf  
        $(Q) $(HEX) [        DISCUZ_CODE_34        ]lt; $@  
  
#用于检查链接脚本和启动文件是否存在，不存在则报错误  
$(START_FILE_SOURCES):  
        $(Q)echo ERROR: The startup file does not exist or has the wrong path !;\  
        exit 1  
$(LDSCRIPT):  
        $(Q)echo ERROR: The link file does not exist or has the wrong path !;\  
        exit 2  
  
clean:  
        $(RM) -rf $(BUILD)  
  
printf:  
        $(Q)echo $(info $(LDFLAGS))  
  
  
JLinkGDBServer:  
        $(Q)JLinkGDBServer -select USB -device $(CHIP) \  
        -endian little -if SWD -speed 4000 -noir -LocalhostOnly  
  
debug:  
        $(Q)make  
        $(Q)echo target remote localhost\:2331 > gdb.gdb  
        $(Q)echo monitor reset >> gdb.gdb  
        $(Q)echo monitor halt >> gdb.gdb  
        $(Q)echo load >> gdb.gdb  
        $(Q)echo b main >> gdb.gdb  
        $(Q)echo - >> gdb.gdb  
        $(Q)echo c >> gdb.gdb  
        $(Q)-$(GDB) $(BUILD)/$(TARGET).elf --command=gdb.gdb  
        $(Q)$(RM) gdb.gdb  
  
download:  
        $(Q)make  
        $(Q)echo "h" > jlink.jlink  
        $(Q)echo "loadfile" $(BUILD)/$(TARGET).hex >> jlink.jlink  
        $(Q)echo "r" >> jlink.jlink  
        $(Q)echo "qc" >> jlink.jlink  
        $(Q)$(JLINKEXE) -device $(CHIP) -Speed 4000 -IF SWD -CommanderScript jlink.jlink  
        $(Q)$(RM) jlink.jlink  
  
reset:  
        $(Q)echo "r" >> jlink.jlink  
        $(Q)echo "qc" >> jlink.jlink  
        $(Q)$(JLINKEXE) -device $(CHIP) -Speed 4000 -IF SWD -CommanderScript jlink.jlink  
        $(Q)$(RM) jlink.jlink  
  
commit:  
        $(Q)git add .  
        $(Q)status='$(shell git status | grep "git pull")';\  
        if test -n "$status";then echo "Need to do git pull !";exit 10;fi  
        $(Q)explain='$(shell read -p "Please input git commit explain:" explain;echo "$explain")';\  
        if test -z "$explain";then git commit -m "Daily development submission"; \  
        else git commit -m "$explain";fi  
        $(Q)git push  
        $(Q)git status  
  

<div>-include $(wildcard $(BUILD)/Obj/*                                       </div><div>
</div>
