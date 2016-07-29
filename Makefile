CC := arm-none-eabi-gcc

ifeq ($(shell uname),Linux)
STELLARIS := /home/l337/egr424/StellarisWare
else
STELLARIS := C:/StellarisWare
endif

BIN := potatOS

CCFLAGS := -Wall -Os -march=armv7-m -mcpu=cortex-m3 -mthumb -mfix-cortex-m3-ldrd -Wl,--gc-sections -I${STELLARIS} -I$(STELLARIS)/boards/ek-lm3s6965
LDFLAGS := -L${STELLARIS}/driverlib/gcc-cm3 -ldriver-cm3 -Tlinkscript.x -Wl,--entry,ResetISR -Wl,--gc-sections

SRCDIR := src
BLDDIR := build
BINDIR := bin

SRCS := $(wildcard $(SRCDIR)/*.c)
OBJS := $(patsubst $(SRCDIR)%.c,$(BLDDIR)%.o, $(SRCS))

all: $(BINDIR)/$(BIN).bin

$(BINDIR)/$(BIN).elf: $(SRCS)
	@mkdir -p $(dir $@)
	${CC} -o $@ $(CCFLAGS) $^ $(LDFLAGS)

%.bin: %.elf
	arm-none-eabi-objcopy -O binary $^ $@

.PHONY: dump
dump: $(BINDIR)/$(BIN).elf program.cfg
	openocd -f program.cfg

.PHONY: clean
clean:
	rm -rf $(BINDIR)/ $(BLDDIR)/
