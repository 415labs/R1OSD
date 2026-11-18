# R1OSD firmware - ATmega324P @ 20 MHz
#
#   make            build build/r1osd.hex (+ .eep, .lss)
#   make size       print flash / RAM usage
#   make program    flash over ISP with avrdude (see PROGRAMMER / PORT)
#   make clean
#
# Compiler flags are the ones used by the original AVR Studio 4 project (2009).

MCU        = atmega324p
F_CPU      = 20000000UL
TARGET     = r1osd
SRCDIR     = src
BUILDDIR   = build

PROGRAMMER ?= avrisp2
PORT       ?= usb

CC      = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE    = avr-size

# rc5.c is #included by rc5-avr.c and must not be compiled on its own.
SRC  = $(filter-out $(SRCDIR)/rc5.c,$(wildcard $(SRCDIR)/*.c))
OBJ  = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRC))
DEP  = $(OBJ:.o=.d)

CFLAGS  = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -std=gnu99 -Wall
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MMD -MP
LDFLAGS = -mmcu=$(MCU) -Wl,-u,vfprintf -Wl,-Map=$(BUILDDIR)/$(TARGET).map
LDLIBS  = -lprintf_flt -lm

all: $(BUILDDIR)/$(TARGET).hex $(BUILDDIR)/$(TARGET).eep $(BUILDDIR)/$(TARGET).lss size

$(BUILDDIR):
	mkdir -p $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/$(TARGET).elf: $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

$(BUILDDIR)/$(TARGET).hex: $(BUILDDIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature $< $@

$(BUILDDIR)/$(TARGET).eep: $(BUILDDIR)/$(TARGET).elf
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
		--change-section-lma .eeprom=0 --no-change-warnings -O ihex $< $@

$(BUILDDIR)/$(TARGET).lss: $(BUILDDIR)/$(TARGET).elf
	$(OBJDUMP) -h -S $< > $@

size: $(BUILDDIR)/$(TARGET).elf
	$(SIZE) --format=avr --mcu=$(MCU) $< 2>/dev/null || $(SIZE) $<

program: $(BUILDDIR)/$(TARGET).hex
	avrdude -p m324p -c $(PROGRAMMER) -P $(PORT) -U flash:w:$<:i

clean:
	rm -rf $(BUILDDIR)

.PHONY: all size program clean

-include $(DEP)
