CC = arm-none-eabi-gcc
LINK = arm-none-eabi-gcc
AS = arm-none-eabi-as
OBJCOPY = arm-none-eabi-objcopy
CTRULIB = e:/git/ctrulib/libctru
CFLAGS += -Wall -std=c99 -march=armv6 -O3 -I"$(CTRULIB)/include" -I$(DEVKITPRO)/libnds/include # -D_DEBUG #define for debug output to sd card
LDFLAGS += -T ccd00.ld -L"$(DEVKITARM)/arm-none-eabi/lib" -L"$(CTRULIB)/lib"

SOURCE = .
BUILD = build
DATA = data

CFILES = $(wildcard $(SOURCE)/*.c)
OFILES = $(CFILES:$(SOURCE)/%.c=$(BUILD)/%.o)
DFILES = $(CFILES:$(SOURCE)/%.c=$(BUILD)/%.d)
SFILES = $(wildcard $(SOURCE)/*.s)
OFILES += $(SFILES:$(SOURCE)/%.s=$(BUILD)/%.o)
BMPFILES = $(wildcard $(DATA)/*.bmp)
OFILES += $(BMPFILES:$(DATA)/%.bmp=$(BUILD)/%.bmp.o)
PROJECTNAME = ${shell basename "$(CURDIR)"}

define bin2o
	bin2s $< | $(AS) -o $(@)
	echo "extern const u8" `(echo $(<F) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`"_end[];" > $(SOURCE)/`(echo $(<F) | tr . _)`.h
	echo "extern const u8" `(echo $(<F) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`"[];" >> $(SOURCE)/`(echo $(<F) | tr . _)`.h
	echo "extern const u32" `(echo $(<F) | sed -e 's/^\([0-9]\)/_\1/' | tr . _)`_size";" >> $(SOURCE)/`(echo $(<F) | tr . _)`.h
endef

.PHONY:=all dir

all: dir $(PROJECTNAME).3ds

dir:
	@mkdir -p $(BUILD)

$(PROJECTNAME).bin: $(PROJECTNAME).elf
	$(OBJCOPY) -O binary $< $@

$(PROJECTNAME).3ds: $(PROJECTNAME).elf
	@make -f make3ds TARGET=$(PROJECTNAME)

$(PROJECTNAME).elf: $(OFILES)
	$(LINK) $(LDFLAGS) -o $(PROJECTNAME).elf $(filter-out $(BUILD)/crt0.o, $(OFILES)) -lctru -lc -nostartfiles

clean:
	@rm -f $(BUILD)/*.o $(BUILD)/*.d
	@rm -f $(PROJECTNAME).elf $(PROJECTNAME).3ds
	@echo "all cleaned up !"

-include $(DFILES)

$(BUILD)/%.o: $(SOURCE)/%.c
	$(CC) $(CFLAGS) -c $< -o $@
	@$(CC) -MM $< > $(BUILD)/$*.d

$(BUILD)/%.o: $(SOURCE)/%.s
	$(CC) $(CFLAGS) -c $< -o $@
	@$(CC) -MM $< > $(BUILD)/$*.d

$(BUILD)/%.bmp.o: $(DATA)/%.bmp
	@echo $(notdir $<)
	@$(bin2o)
