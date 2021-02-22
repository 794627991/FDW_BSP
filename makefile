#*************************************************************************  
#  **  
#  ** File         : Makefile  
#  ** Abstract     : This is the introduction to the document  
#  ** Author       : wr  
#  ** mail         : 791314247@q.com  
#  ** Created Time : 2020��11��22�� ������ 11ʱ58��06��  
#  ** copyright    : COPYRIGHT(c) 2020  
#  **  
#  ************************************************************************/  
  
ifneq ($(V),1)  
Q := @  
else  
Q :=  
endif  
  
################################������Ŀ���û�������Ҫ����##########################  
# ����ļ������ƣ�Ĭ��Ϊmain(main.elf main.bin main.hex)  
TARGET := main  
  
#�����ļ����ƺ�����·��  
LDSCRIPT := ./STM32F429IGTx_FLASH.ld  
  
#�����ļ����ƺ�����·��  
START_FILE_SOURCES := ./startup_stm32f429xx.s  
  
#�ں�ѡ��FPU, FLOAT-ABI��Ϊ��  
CPU       := -mcpu=cortex-m4  
FPU       := -mfpu=fpv4-sp-d16  
FLOAT-ABI := -mfloat-abi=hard  
  
#ϵͳ�궨��  
C_DEFS    := \  
-DUSE_HAL_DRIVER \  
-DSTM32F429xx  
  
# оƬ�ͺţ�����Jlink������ԡ�����  
CHIP      := STM32F429IG  
  
# ѡ���Ż��ȼ�:  
# 1. gcc��ָ���Ż�����Ĳ����У�-O0��-O1��-O2��-O3��-Og��-Os��-Ofast��  
# 2. �ڱ���ʱ�����û��ָ��������κ��Ż���������Ĭ��Ϊ -O0����û���Ż���  
# 3. ���� -O1��-O2��-O3 �У��������ֱ�󣬴�����Ż��̶�ҲԽ�ߣ���������ĳ����������˵��Ҳ������������Ŀɵ�����Ϊ���۵ġ�  
# 4. ���� -Og ���� -O1 �Ļ����ϣ�ȥ������ЩӰ����Ե��Ż����������������Ϊ�˵��Գ��򣬿���ʹ��������������������������Ҳ�ǲ��еģ��������ֻ�Ǹ��߱������������Ĵ��벻ҪӰ����ԣ���������Ϣ�����ɻ��ǿ� -g �����ġ�  
# 5. ���� -Os ���� -O2 �Ļ����ϣ�ȥ������Щ�ᵼ�����տ�ִ�г���������Ż��������Ҫ��С�Ŀ�ִ�г��򣬿�ѡ�����������  
# 6. ���� -Ofast ���� -O3 �Ļ����ϣ������һЩ�ǳ����Ż�����Щ�Ż���ͨ������һЩ���ʱ�׼������һЩ��ѧ������ʵ�ֱ�׼����ʵ�ֵģ�����һ�㲻�Ƽ�ʹ�øò�����  
# 7. �����֪��������Ż���������������Щ�Ż�������ʹ�� gcc -Q --help=optimizers ��������ѯ��  
OPT       := -Og  
  
# �Ƿ�debug��Ϣ�����.elf�ļ���Ĭ�ϴ�  
DEBUG     := 1  
  
# ����ļ��У�.hex .bin .elf���ڴ��ļ����£�.o .d�ļ����ڴ��ļ�����Ŀ¼Obj��(�Զ�����)  
BUILD     := ./build  
  
# ֧��˫ϵͳ���룬����ѡ��ǰϵͳ��0Ϊlinux��1Ϊwindows  
SYS    := 1  
# ��ָ����windowsϵͳ������ȷ�ϱ�������·��������װʱ��Ĭ��·����װ������ȷ  
ifeq ($(SYS), 1)  
GCC_PATH = "/c/Program Files (x86)/GNU Tools Arm Embedded/9 2019-q4-major/bin"  
JLINK_PATH = "/c/Program Files (x86)/SEGGER/JLink"  
endif  
###################################�û��޸Ľ���###################################  
  
# ����������  
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
  
# Jlink���壬����֧��һ�����غ�gdb����  
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
  
#�������е�h�ļ�,�����Я��-I��.h�ļ�·��  
C_INCLUDES := $(addprefix -I,$(subst ./,,$(sort $(dir $(shell find ./ -type f -iname "*.h")))))  
  
#�������  
CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -fdata-sections -ffunction-sections  
#���ؾ���  
CFLAGS += -Wall -W -Wno-unused-parameter  
#��׼  
CFLAGS += -std=c99  
  
#������DEBUG����ʱЯ��DEBUG����  
ifeq ($(DEBUG), 1)  
CFLAGS += -g -gdwaRF-2  
endif  
  
#�Զ����������ļ�  
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"  
#################### CFLAGS Config End ##########################  
  
# libraries  
LIBS = -lc -lm -lnosys  
LIBDIR =  
#����ָ�-specs=nosys.specs  
LDFLAGS = $(MCU) -T$(LDSCRIPT) -specs=nano.specs $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD)/Obj/$(TARGET).map,--cref  
#�Ƿ����Ż���δʹ�õĺ����ͷ���  
LDFLAGS += -Wl,--gc-sections  
  
#���������ļ�����Obj,���ȥ��·����.o�ļ�,�ɼ���.s��.S  
START_FILE_OBJ     = $(addsuffix .o, $(basename $(notdir $(START_FILE_SOURCES))))  
OBJECTS            = $(addprefix $(BUILD)/Obj/, $(START_FILE_OBJ))  
  
#�������е�c�ļ����������е�.c�ļ�����Obj  
C_SOURCES          = $(subst ./,,$(shell find ./ -type f -iname "*.c"))  
OBJECTS           += $(addprefix $(BUILD)/Obj/, $(notdir $(C_SOURCES:%.c=%.o)))  
#PS:ȥ���ռ�Ŀ���ԭʼ·��ǰ׺���������ļ���·��ǰ׺(�ı��������ļ���·��ǰ׺����Ҫ����ָ������·��)  
  
#ָ��makefile�����ļ���·��(�����ռ�Ŀ��������ļ���Я��.c�ļ����ڵ�·����  
#�Ҳ�ָ������·����makefile�ᱨ��û�й����ƶ�Ŀ��)  
vpath %.c $(sort $(dir $(C_SOURCES)))  #ȡ��·����ȥ�غ�����(������ĸΪ��λ)  
vpath %.s $(dir $(START_FILE_SOURCES))  
vpath %.S $(dir $(START_FILE_SOURCES))  
  
  
#ָ��ΪαĿ������������������������makefile�����ܣ�����ֹmakeʱЯ���Ĳ�����ʵ���ļ�����������  
.PHONY:all clean printf JLinkGDBServer debug download reset commit  
  
all : $(BUILD)/$(TARGET).elf $(BUILD)/$(TARGET).bin $(BUILD)/$(TARGET).hex  
  
  
#�������е�.o����.elf�ļ�  
$(BUILD)/$(TARGET).elf : $(OBJECTS) | $(LDSCRIPT)  
        $(Q)$(CC) $(LDFLAGS) -o $[url=home.php?mod=space&uid=72445]@[/url] $(OBJECTS)  
        $(Q)echo "make $@:"  
        $(Q)$(SZ) $@  
  
  
#���������ļ�  ���ò�����#-x assembler-with-cpp  
$(BUILD)/Obj/$(START_FILE_OBJ) : $(START_FILE_SOURCES) Makefile | $(BUILD)/Obj  
        $(Q)echo "buid $(subst ./,,[        DISCUZ_CODE_34        ]lt;)"  
        $(Q) $(CC) -c $(CFLAGS) -o $@ [        DISCUZ_CODE_34        ]lt;  
  
#���빤��  
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
  
#���ڼ�����ӽű��������ļ��Ƿ���ڣ��������򱨴���  
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
