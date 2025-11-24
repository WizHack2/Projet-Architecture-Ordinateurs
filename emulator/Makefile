TARGET  = emulator
BUILD   = build
SRC     = $(wildcard *.c)
OBJ     = $(addprefix $(BUILD)/, $(SRC:.c=.o))
DEPS    = $(OBJ:.o=.d)
CFLAGS += -W -Wall
CFLAGS += -O0 -g
LDFLAGS = 

all: $(BUILD)/$(TARGET)

.PHONY: clean exec gdb

-include $(DEPS)

$(BUILD)/%.o: %.c
	@mkdir -p $(@D)
	gcc $(CFLAGS) -c -o $@ $< -MMD -MP -MF"$(@:%.o=%.d)"

$(BUILD)/$(TARGET): $(OBJ)
	gcc $(CFLAGS) -o $@ $^ $(LDFLAGS)

exec: $(BUILD)/$(TARGET)
	./$<

gdb: $(BUILD)/$(TARGET)
    gdb --tui $<

clean:
	@rm -rvf $(BUILD)