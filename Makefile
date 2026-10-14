# Formula Retro - Nintendo DS / DSi (devkitPro / libnds)
ifeq ($(strip $(DEVKITPRO)),)
$(error DEVKITPRO is not set. Install devkitPro nds-dev and source its environment)
endif
ifeq ($(strip $(DEVKITARM)),)
$(error DEVKITARM is not set. Install devkitPro nds-dev and source its environment)
endif
include $(DEVKITARM)/ds_rules

TARGET := formula_retro
BUILD := build
SOURCES := source
INCLUDES := source

ARCH := -mthumb -mthumb-interwork
CFLAGS := -g -Wall -Wextra -O2 -ffunction-sections -fdata-sections $(ARCH) $(INCLUDE) -DARM9
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions
ASFLAGS := -g $(ARCH)
LDFLAGS := -g $(ARCH) -Wl,-Map,$(notdir $*.map)
LIBS := -lnds9
LIBDIRS := $(LIBNDS)

ifneq ($(BUILD),$(notdir $(CURDIR)))
export OUTPUT := $(CURDIR)/$(TARGET)
export VPATH := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
export DEPSDIR := $(CURDIR)/$(BUILD)
CFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
export OFILES := $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) $(SFILES:.s=.o)
export INCLUDE := $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) $(foreach dir,$(LIBDIRS),-I$(dir)/include)
export LIBPATHS := $(foreach dir,$(LIBDIRS),-L$(dir)/lib)
.PHONY: all clean
all: $(BUILD)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
$(BUILD):
	@mkdir -p $@
clean:
	@rm -rf $(BUILD) $(TARGET).nds $(TARGET).elf $(TARGET).map
else
DEPENDS := $(OFILES:.o=.d)
.PHONY: all
all: $(OUTPUT).nds
$(OUTPUT).nds: $(OUTPUT).elf
$(OUTPUT).elf: $(OFILES)
-include $(DEPENDS)
endif
