default: all
include ./GCC/makefile_*

ifeq ($(OS),Windows_NT)
	PYTHON := python
else
	PYTHON := python3
endif

###############################################################################
# Main application file name
###############################################################################
TARGET := $(notdir $(shell pwd))
VERSION_STR := $(shell $(PYTHON) scripts/get_version.py)
PHONY :=
MAKEFLAGS += --silent --g 4



###############################################################################
# Build path
###############################################################################
ifeq ($(RELEASE), 1)
BUILD_DIR := build/release
else
BUILD_DIR := build/debug
DEBUG := 1
endif

SRC_DIR := code
LIB_FOLDER := Drivers
DSP_FOLDER := 
EXC_FOLDER := temp


###############################################################################
# Source
###############################################################################
# Defines
DEFINES_C := STM32F407xx
DEFINES_C += USE_FULL_LL_DRIVER
DEFINES_C += USE_HAL_DRIVER
DEFINES_C += HSE_VALUE=25000000UL
DEFINES_C += VERSION_STR=\"$(VERSION_STR)\"

# includes
INCLUDES := $(shell find $(SRC_DIR) -type d -not -path $(EXC_FOLDER))
INCLUDES_LIB := $(shell find $(SRC_DIR)/$(LIB_FOLDER) -type d -not -path $(EXC_FOLDER))
INCLUDES_SRC := $(filter-out $(INCLUDES_LIB),$(INCLUDES))
INCLUDES := $(INCLUDES_SRC) $(INCLUDES_LIB)

INCLUDES_CC := ./GCC

# ASM sources
SOURCES_A := startup_stm32f407xx.s

# C sources
SOURCES_C := $(wildcard $(addsuffix /*.c, $(INCLUDES_SRC)))

# C lib sources
SOURCES_C_LIB := $(wildcard $(addsuffix /*.c, $(INCLUDES_LIB)))

INCLUDES += $(dir $(SOURCES_C_LIB))

###############################################################################
# Compilers and Utilities binaries
###############################################################################
PREFIX = arm-none-eabi-
CC_PATH := $(GCC_ARM_NONE_EABI_PATH)

AS 		:= "$(CC_PATH)$(PREFIX)gcc" -xassembler-with-cpp
CC 		:= "$(CC_PATH)$(PREFIX)gcc"
LD 		:= "$(CC_PATH)$(PREFIX)gcc"
OBJCOPY := "$(CC_PATH)$(PREFIX)objcopy"
OBJDUMP := "$(CC_PATH)$(PREFIX)objdump"
SZ 		:= "$(CC_PATH)$(PREFIX)size"
CC_VERSION := $(shell $(CC) --version)


###############################################################################
# CFLAGS
###############################################################################
# CPU
CPU := -mcpu=cortex-m4

# FPU
FPU := -mfpu=fpv4-sp-d16 -mfloat-abi=hard

# MCU
MCU := $(CPU) $(FPU) -mthumb

# Optimization and Debug level
ifeq ($(DEBUG), 1)
DEFINES_C += DEBUG
OPT := -O1 --debug
else
DEFINES_C += DEBUG
OPT := -Os
endif

# Compile flags
FLAGS := $(MCU)

AFLAGS := $(FLAGS)

FLAGS_C := $(FLAGS)
FLAGS_C += -ffunction-sections
FLAGS_C += -fdata-sections
FLAGS_C += -funsigned-char
FLAGS_C += -fpeel-loops
FLAGS_C += -std=c2x
FLAGS_C += -fcx-limited-range
FLAGS_C += $(OPT)
FLAGS_C += -MMD -MP

FLAGS_C_W := -Wall
FLAGS_C_W += -Winline
FLAGS_C_W += -Wextra
FLAGS_C_W += -pedantic

FLAGS_DEF := $(addprefix -D, $(DEFINES_C))
FLAGS_INC := $(addprefix -I, $(INCLUDES)) $(addprefix -I, $(INCLUDES_CC))


###############################################################################
# FLAGS_LD
###############################################################################
# link script
LSCRIPT := GCC/STM32F407VETx_FLASH.ld

# libraries
LIBS := -lc -lm -lnosys 
LIBDIR := 
FLAGS_LD := $(MCU) -T$(LSCRIPT) $(LIBDIR) $(LIBS)
FLAGS_LD += --specs=nano.specs 
FLAGS_LD += --specs=nosys.specs 
# FLAGS_LD += -u _scanf_float 
# FLAGS_LD += -u _printf_float
FLAGS_LD += -Wl,--no-warn-rwx-segments
FLAGS_LD += -Wl,--gc-sections
FLAGS_LD += -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref 


###############################################################################
# FLAGS_H
###############################################################################
# Options for HEX file generation
FLAGS_H := -O ihex

###############################################################################
# build the application
###############################################################################
OBJECTS := $(addprefix $(BUILD_DIR)/, $(notdir $(SOURCES_C:.c=.o)))
OBJECTS += $(addprefix $(BUILD_DIR)/, $(notdir $(SOURCES_C_LIB:.c=.lo)))
OBJECTS += $(addprefix $(BUILD_DIR)/, $(notdir $(SOURCES_A:.s=.o)))
vpath %.c $(sort $(INCLUDES))
vpath %.s $(sort $(INCLUDES)) GCC

# Build all
all: build
	@echo
	$(SZ) $(BUILD_DIR)/$(TARGET).elf
	@echo
	$(call echo_green,"Listing:") $(BUILD_DIR)/$(TARGET).lss

build: info $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).lss

rebuild: clean build

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(OBJCOPY) $(FLAGS_H) $< $@

$(BUILD_DIR)/%.lss: $(BUILD_DIR)/%.elf | $(BUILD_DIR)	
	$(OBJDUMP) -S $< > $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)	
	$(CC) $(OBJECTS) $(FLAGS_LD) -o $@

$(BUILD_DIR)/%.lo: %.c | $(BUILD_DIR)
	@echo $<
	$(CC) -c $(FLAGS_C) $(FLAGS_INC) $(FLAGS_DEF) $< -o $@

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo $<
	$(CC) -c $(FLAGS_C) $(FLAGS_C_W) $(FLAGS_INC) $(FLAGS_DEF) $< -o $@

$(BUILD_DIR)/%.o: %.s | $(BUILD_DIR)
	@echo $<
	$(AS) -c $(AFLAGS) $< -o $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	@echo -rm -fR $(dir $(BUILD_DIR))
	@-rm -fR $(dir $(BUILD_DIR))

utility="C:\Program Files (x86)\STMicroelectronics\STM32 ST-LINK Utility\ST-LINK Utility\ST-LINK_CLI.exe"
flash: $(BUILD_DIR)/$(TARGET).hex
	$(utility) -P $< 0x08000000
	$(utility) -Rst -Run >&-
	@echo $(COLOR_GREEN)"Appication Run"$(COLOR_NC)

erase:
	$(utility) -ME >&-
	@echo $(COLOR_GREEN)"Flash memory erased"$(COLOR_NC)

info:
	$(call echo_green,"Target:  ") $(TARGET)
	$(call echo_green,"Version: ") $(VERSION_STR)
	$(call echo_green,"Build to:") $(BUILD_DIR)
	$(call echo_green,Compiler version:) "$(CC_VERSION)"
	$(call echo_green,Compiler flags:) $(FLAGS_C) $(FLAGS_C_W)
	$(call echo_green,Defines:) $(DEFINES_C)
	$(call echo_green,Linking flags:) $(FLAGS_LD)
	@echo

PHONY += test
test :
	@echo $(CC)
	@echo ${OBJDUMP}


###############################################################################
# dependencies
###############################################################################
-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: $(PHONY)
