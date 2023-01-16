###############################################################################
# Main application file name
###############################################################################
TARGET := $(notdir $(shell pwd))
PHONY :=
MAKEFLAGS += --silent


###############################################################################
# Build path
###############################################################################
ifeq ($(RELEASE), 1)
BUILD_DIR := build/release
else
BUILD_DIR := build/debug
DEBUG := 1
endif


###############################################################################
# Build version
###############################################################################
# GIT_TAGS := $(shell git show-ref --tags --abbrev)
# GIT_TAG_COMMIT := $(word $(words $(GIT_TAGS)), abc $(GIT_TAGS))
# GIT_TAG := $(notdir $(lastword $(GIT_TAGS)))
# GIT_COMMIT := $(firstword $(shell git show-ref --heads --abbrev))

# VERSION := $(subst v,,$(GIT_TAG))
# VERSION_NUM := $(subst ., ,$(VERSION))
# ifeq ($(words $(VERSION_NUM)), 3)
# 	VERSION_MAJOR := $(word 1, $(VERSION_NUM))
# 	VERSION_MINOR := $(word 2, $(VERSION_NUM))
# 	VERSION_REV := $(word 3, $(VERSION_NUM))
# 	VERSION_NUM := $$((\
# 		$(VERSION_MAJOR) << 16 + $(VERSION_MINOR) << 8 + $(VERSION_REV)\
# 	))
# else ifeq ($(words $(VERSION_NUM)), 2)
# 	VERSION_MAJOR := $(word 1, $(VERSION_NUM))
# 	VERSION_MINOR := $(word 2, $(VERSION_NUM))
# 	VERSION_NUM := $$((\
# 		$(VERSION_MAJOR) * 256 + $(VERSION_MINOR)\
# 	))
# else
# 	VERSION_NUM := 0
# endif

# VERSION_COMMIT := 0x$(shell echo $(GIT_COMMIT) | tr a-f A-F)
# VERSION_STR := $(VERSION)-$(VERSION_COMMIT)


###############################################################################
# Source
###############################################################################
# Defines
C_DEFINES := STM32F407xx
C_DEFINES += USE_FULL_LL_DRIVER
C_DEFINES += USE_HAL_DRIVER

# includes
INCLUDES := $(shell find code -type d)

# ASM sources
A_SOURCES := $(wildcard $(addsuffix /*.s, $(INCLUDES)))

# C sources
C_SOURCES := $(wildcard $(addsuffix /*.c, $(INCLUDES)))


###############################################################################
# Compilers and Utilities binaries
###############################################################################
PREFIX = arm-none-eabi-
CC_PATH ?= C:/toolchain/gcc-arm-none-eabi-10.3-2021.10/bin

AS 		:= $(CC_PATH)/$(PREFIX)gcc -xassembler-with-cpp
CC 		:= $(CC_PATH)/$(PREFIX)gcc.exe
LD 		:= $(CC_PATH)/$(PREFIX)armlink
OBJCOPY := $(CC_PATH)/$(PREFIX)objcopy
OBJDUMP := $(CC_PATH)/$(PREFIX)objdump
SZ 		:= $(CC_PATH)/$(PREFIX)size
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
C_DEFINES += DEBUG
OPT := -O2 --debug
else
OPT := -Os
endif

# Compile flags
FLAGS := $(MCU)
FLAGS += -ffunction-sections
FLAGS += -fdata-sections
FLAGS += -fshort-enums
FLAGS += -funsigned-char
FLAGS += -fpeel-loops
FLAGS += -std=gnu17
FLAGS += -Wall

AFLAGS := $(FLAGS)

CFLAGS := $(FLAGS)
CFLAGS += $(OPT)
CFLAGS += -MMD -MP

FLAGS_DEF := $(addprefix -D, $(C_DEFINES))
FLAGS_INC := $(addprefix -I, $(INCLUDES)) $(addprefix -I, $(INCLUDES_CC))


###############################################################################
# LFLAGS
###############################################################################
# link script
LSCRIPT := STM32F407VETx_FLASH.ld

# libraries
LIBS := -lc -lm -lnosys 
LIBDIR := 
LFLAGS := $(MCU) -T$(LSCRIPT) $(LIBDIR) $(LIBS)
LFLAGS += -specs=nano.specs 
# LFLAGS += -u _scanf_float 
# LFLAGS += -u _printf_float
LFLAGS += -Wl,--gc-sections
LFLAGS += -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref 


###############################################################################
# HFLAGS
###############################################################################
# Options for HEX file generation
HFLAGS := -O ihex

###############################################################################
# build the application
###############################################################################
OBJECTS := $(addprefix $(BUILD_DIR)/, $(notdir $(C_SOURCES:.c=.o)))
OBJECTS += $(addprefix $(BUILD_DIR)/, $(notdir $(A_SOURCES:.s=.o)))
vpath %.c $(sort $(INCLUDES))
vpath %.s $(sort $(INCLUDES))

# Build all
all: build
	@echo
	$(SZ) $(BUILD_DIR)/$(TARGET).elf
	@echo
	@echo -e "\033[32;1;40m""Listing:""\033[0m" $(BUILD_DIR)/$(TARGET).lss

build: check_cc info $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).lss

rebuild: clean build

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile	
	$(CC) $(OBJECTS) $(LFLAGS) -o $@
	
	
$(BUILD_DIR)/%.lss: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(OBJDUMP) -S $< > $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(OBJCOPY) $(HFLAGS) $< $@

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	@echo $<
	$(CC) -c $(CFLAGS) $(FLAGS_INC) $(FLAGS_DEF) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
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
	@echo
	$(call echo_yellow,"Target:  ") $(TARGET)
	$(call echo_yellow,"Version: ") $(VERSION_STR)
	$(call echo_yellow,"Build to:") $(BUILD_DIR)
	$(call echo_green,"Compiler version:") "$(CC_VERSION)"
	$(call echo_green,Compiler flags:) $(CFLAGS)
	# $(call echo_green,Includes folder:) $(INCLUDES) $(INCLUDES_CC)
	$(call echo_green,Defines:) $(C_DEFINES)
	$(call echo_green,Linking flags:) $(LFLAGS)
	@echo

PHONY += test
test :
	@echo ${BUILD_DIR}
	@echo ${OBJECTS}

PHONY += check_cc
check_cc:
ifeq ($(wildcard $(CC_PATH)),)
	$(error Compiler path: $(CC_PATH) not found)
endif

PHONY += version
version:
	@$(call echo_yellow,"GIT_TAG:        ") $(GIT_TAG)
	@$(call echo_yellow,"GIT_TAG_COMMIT: ") $(GIT_TAG_COMMIT)
	@$(call echo_yellow,"GIT_COMMIT:     ") $(GIT_COMMIT)
	@$(call echo_yellow,"VERSION:        ") $(VERSION)
	@$(call echo_yellow,"VERSION_NUM:    ") $(VERSION_NUM)
	@$(call echo_yellow,"VERSION_COMMIT: ") $(VERSION_COMMIT)
	@$(call echo_yellow,"VERSION_STR:    ") $(VERSION_STR)

check_version:
ifneq ($(GIT_TAG_COMMIT),$(GIT_COMMIT))
	$(call echo_cyan,Warning:) \
		"Git tag commit $(GIT_TAG_COMMIT) is not eqval last commit $(GIT_COMMIT)"
endif


###############################################################################
# dependencies
###############################################################################
-include $(wildcard $(BUILD_DIR)/*.d)


###############################################################################
# Colors for echo -e
###############################################################################
COLOR_BLACK		:= "\033[31m"
COLOR_RED		:= "\033[31m"
COLOR_GREEN		:= "\033[32m"
COLOR_YELLOW	:= "\033[33m"
COLOR_BLUE		:= "\033[34m"
COLOR_MAGENTA	:= "\033[35m"
COLOR_CYAN		:= "\033[36m"
COLOR_WHITE		:= "\033[37m"
COLOR_NC		:= "\033[0m"

###############################################################################
# Colors echo functions
###############################################################################
echo_black = @echo -e $(COLOR_BLACK)$(1)$(COLOR_NC)
echo_red = @echo -e $(COLOR_RED)$(1)$(COLOR_NC)
echo_green = @echo -e $(COLOR_GREEN)$(1)$(COLOR_NC)
echo_yellow = @echo -e $(COLOR_YELLOW)$(1)$(COLOR_NC)
echo_blue = @echo -e $(COLOR_BLUE)$(1)$(COLOR_NC)
echo_magenta = @echo -e $(COLOR_MAGENTA)$(1)$(COLOR_NC)
echo_cyan = @echo -e $(COLOR_CYAN)$(1)$(COLOR_NC)
echo_white = @echo -e $(COLOR_WHITE)$(1)$(COLOR_NC)

.PHONY: $(PHONY)
