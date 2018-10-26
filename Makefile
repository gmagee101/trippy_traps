MSG_FLASH=Creating final hex file and flashing to Teensy
MCU=at90usb1286
FORMAT=ihex
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
TARGET = demo
SRC =	src/main.c \
		src/LightUtilities.c \
		src/print.c \
		src/usb_debug_only.c \
		src/i2c.c \
		src/LSM9DSO.c \
		src/LightBehaviors.c

OBJDIR = ./bin
OBJ = $(SRC:src/%.c=$(OBJDIR)/%.o)

F_CPU = 8000000
DEBUG = dwarf-2

CFLAGS = -g
CFLAGS += -DF_CPU=$(F_CPU)UL
CFLAGS += -Os
CFLAGS += -funsigned-char
CFLAGS += -funsigned-bitfields
CFLAGS += -ffunction-sections
CFLAGS += -fpack-struct
CFLAGS += -fshort-enums
CFLAGS += -Wall
CFLAGS += -Wstrict-prototypes
CFLAGS += -std=gnu99
CFLAGS += -Isrc/


#---------------- Linker Options ----------------
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
EXTRALIBDIRS = /usr/local/lib \
			   /usr/lib/ \

LDFLAGS = -Wl,-Map=$(TARGET).map,--cref
LDFLAGS += -Wl,--relax
LDFLAGS += -Wl,--gc-sections
LDFLAGS += $(patsubst %,-L%,$(EXTRALIBDIRS))

# Compiler flags to generate dependency files.
GENDEPFLAGS = -MMD -MP -MF .dep/$(@F).d

ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS)

default: $(TARGET).hex

# Link: create ELF output file from object files.
%.elf: $(OBJ)
	@echo Linking $@
	@ $(CC) $(ALL_CFLAGS) $^ --output $@ $(LDFLAGS)

# Compile: create object files from C source files.
$(OBJDIR)/%.o : src/%.c $(OBJDIR)
	@echo compiling $@
	@ $(CC) -c $(ALL_CFLAGS) -Wa,-adhlns=$(<:src/%.c=$(OBJDIR)/%.lst) $< -o $@

# Create final output files (.hex, .eep) from ELF output file.
%.hex: %.elf
	@echo transferring output to $@
	@ $(OBJCOPY) -O $(FORMAT) -R .eeprom -R .fuse -R .lock -R .signature $< $@

flash: $(TARGET).hex
	@echo flashing board
	@teensy_loader_cli -mmcu=$(MCU) -w -v $(TARGET).hex

src/i2c.c : lib/avr-i2c/i2c.c
	cp lib/avr-i2c/i2c.* ./src/

$(OBJDIR) :
	@echo creating object directory
	@mkdir -p $(OBJDIR)
	

clean:
	rm -rf $(TARGET).hex
	rm -rf $(TARGET).map
	rm -rf $(TARGET).elf
	rm -rf src/i2c.*
	rm -rf $(OBJDIR)
