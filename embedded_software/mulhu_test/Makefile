# Name of the output program
TARGET  = esw
# Name of the build directory
BUILD   = build
# Base name of the toolchain
TC      = riscv32-MINIRISC-elf
CC      = $(TC)-gcc
LD      = $(TC)-gcc
SIZE    = $(TC)-size
OBJCOPY = $(TC)-objcopy
OBJDUMP = $(TC)-objdump

CFLAGS  += -march=rv32im_zicsr
CFLAGS  += -W -Wall
CFLAGS  += -O2

LDFLAGS += -nostartfiles
LDFLAGS += -Wl,-Ttext=0x80000000

SRCS   += $(wildcard *.S)
OBJS    = $(addprefix $(BUILD)/, $(SRCS:.S=.o))
DEPS    = $(OBJS:.o=.d)

.PHONY: all clean lss

all: $(BUILD)/$(TARGET).bin

-include $(DEPS)

$(BUILD)/%.o: %.S
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -x assembler-with-cpp -c $< -o $@ -MMD -MP -MF"$(@:%.o=%.d)"

$(BUILD)/$(TARGET).elf: $(OBJS)
	$(LD) -o $@ $(filter %.o,$^) $(CFLAGS) $(LDFLAGS)  
	@echo "────────────────────────────────────────────────────────────────────────"
	@$(SIZE) $@
	@echo "────────────────────────────────────────────────────────────────────────"

$(BUILD)/$(TARGET).bin: $(BUILD)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(BUILD)/$(TARGET).lss: $(BUILD)/$(TARGET).elf
	$(OBJDUMP) -h -D $< > $@

lss: $(BUILD)/$(TARGET).lss
	less $<

clean:
	@rm -rf $(BUILD)