ifndef PICO_SDK_PATH
$(error Please define PICO_SDK_PATH)
endif

# Input file names
WIFI_LAUNCH_FW_NAME=launch_firmware/43439A0.bin
BT_FW_SRC=cybt_firmware_43439.c

WIFI_LATEST_FW_NAME?=wb_firmware_jan2023/43439A0.bin
CLM_NAME=launch_tuning/43439_raspberrypi_picow_v5_220624.clm_blob

# Intermediate files for BT firmware
BT_FW_BIN_NAME=bt_fw_processor.out
BT_FW_NAME=cyw43_btfw_43439

BT_FW_HCD_IN=CYW4343A2_001.003.016.0066.0000_Generic_SDIO_37_4MHz_wlbga_BU_dl_signed.hcd
BT_FW_READ_HCD_BIN_NAME=read_hcd
BT_FW_NAME_HCD=cyw43_btfw_43439_hcd

# Filename for the combined firmware
WB_FIRMWARE_COMBINED=wb43439A0_7_95_49_00_combined
LAUNCH_FIRMWARE_COMBINED=w43439A0_7_95_49_00_combined

# This is the A1 (STM32?) firmware
WIFI_A1_FW_NAME=a1_firmware/43439A1.bin
CLM_A1_NAME=a1_firmware/43439A1.clm_blob
BT_A1_FW_NAME=cyw43_btfw_4343A1
A1_FIRMWARE_COMBINED=w4343WA1_7_45_98_50_combined

# NVRAM files
NVRAM_1DX=wifi_nvram_1dx
NVRAM_PICO=wifi_nvram_43439

TGZIP_BIN_NAME=tgzip.out
MINIZ_BIN_NAME=miniz/bin/miniz_gzip

RESULT_LIST = \
	$(LAUNCH_FIRMWARE_COMBINED).h \
	$(LAUNCH_FIRMWARE_COMBINED).gz.h \
	$(WB_FIRMWARE_COMBINED).h \
	$(WB_FIRMWARE_COMBINED).gz.h \
	$(A1_FIRMWARE_COMBINED).h \
	$(A1_FIRMWARE_COMBINED).gz.h \
	$(BT_FW_NAME).h \
	$(BT_FW_NAME).gz.h \
	$(BT_A1_FW_NAME).h \
	$(BT_A1_FW_NAME).gz.h \
	$(BT_FW_NAME_HCD).h \
	$(BT_FW_NAME_HCD).gz.h \

# nvram data is not compressed
# So it would be a bit of hassle to decompress to save one page?
#RESULT_LIST += \
#	$(NVRAM_1DX).gz.h \
#	$(NVRAM_PICO).gz.h

.PHONY: result
result: $(foreach FILE,$(RESULT_LIST),$(PICO_SDK_PATH)/lib/cyw43-driver/firmware/$(FILE))

#1: wifi fw binary
#2: clm binary
#3: result name
define add_wifi_meta
	cp $(3).premeta.h $(3).postmeta.h
	@echo \#define CYW43_WIFI_FW_LEN \($$(stat -c %s $(1))\) // $(1)>>$(3).postmeta.h
	@echo \#define CYW43_CLM_LEN \($$(stat -c %s $(2))\) // $(2)>>$(3).postmeta.h
	@echo \#define CYW43_WIFI_FW_ADDR \($(subst .,_,$(3))\)>>$(3).postmeta.h
	@echo \#define CYW43_RAW_WIFI_FW_LEN \($(subst .,_,$(3))_len\)>>$(3).postmeta.h
	@echo const uintptr_t fw_data = \(uintptr_t\)CYW43_WIFI_FW_ADDR\;>>$(3).postmeta.h
endef

#1: bt fw binary
#2: result name
define add_bt_meta
	cp $(2).btpremeta.h $(2).postmeta.h
	@echo \#define CYW43_BT_FW_LEN \($$(stat -c %s $(1))\)>>$(2).postmeta.h
	@echo \#define CYW43_BT_FW_ADDR \($(subst .,_,$(2))\)>>$(2).postmeta.h
	@echo \#define CYW43_RAW_BT_FW_LEN \($(subst .,_,$(2))_len\)>>$(2).postmeta.h
endef

#1: nvram binary
#2: result name
define add_nvram_meta
	cp $(2).premeta.h $(2).postmeta.h
	@echo \#define CYW43_NVRAM_FW_LEN \($$(stat -c %s $(1))\)>>$(2).postmeta.h
	@echo \#define CYW43_NVRAM_FW_ADDR \($(subst .,_,$(2))\)>>$(2).postmeta.h
	@echo \#define CYW43_RAW_NVRAM_FW_LEN \($(subst .,_,$(2))_len\)>>$(2).postmeta.h
endef

# Copy to destination
$(PICO_SDK_PATH)/lib/cyw43-driver/firmware/%.h : %.postmeta.h
	cp $< $@

# add metadata
$(LAUNCH_FIRMWARE_COMBINED).postmeta.h : $(LAUNCH_FIRMWARE_COMBINED).premeta.h
	$(call add_wifi_meta,$(WIFI_LAUNCH_FW_NAME),$(CLM_NAME),$(subst .postmeta.h,,$@))

$(LAUNCH_FIRMWARE_COMBINED).gz.postmeta.h : $(LAUNCH_FIRMWARE_COMBINED).gz.premeta.h
	$(call add_wifi_meta,$(WIFI_LAUNCH_FW_NAME),$(CLM_NAME),$(subst .postmeta.h,,$@))

$(WB_FIRMWARE_COMBINED).postmeta.h : $(WB_FIRMWARE_COMBINED).premeta.h
	$(call add_wifi_meta,$(WIFI_LATEST_FW_NAME),$(CLM_NAME),$(subst .postmeta.h,,$@))

$(WB_FIRMWARE_COMBINED).gz.postmeta.h : $(WB_FIRMWARE_COMBINED).gz.premeta.h
	$(call add_wifi_meta,$(WIFI_LATEST_FW_NAME),$(CLM_NAME),$(subst .postmeta.h,,$@))

$(A1_FIRMWARE_COMBINED).postmeta.h : $(A1_FIRMWARE_COMBINED).premeta.h
	$(call add_wifi_meta,$(WIFI_A1_FW_NAME),$(CLM_A1_NAME),$(subst .postmeta.h,,$@))

$(A1_FIRMWARE_COMBINED).gz.postmeta.h : $(A1_FIRMWARE_COMBINED).gz.premeta.h
	$(call add_wifi_meta,$(WIFI_A1_FW_NAME),$(CLM_A1_NAME),$(subst .postmeta.h,,$@))

$(BT_FW_NAME).postmeta.h: $(BT_FW_NAME).btpremeta.h
	$(call add_bt_meta,$(subst .postmeta.h,,$@).raw,$(subst .postmeta.h,,$@))

$(BT_FW_NAME).gz.postmeta.h: $(BT_FW_NAME).gz.btpremeta.h
	$(call add_bt_meta,$(subst .gz.postmeta.h,,$@).raw,$(subst .postmeta.h,,$@))

$(BT_FW_NAME_HCD).postmeta.h: $(BT_FW_NAME_HCD).btpremeta.h
	$(call add_bt_meta,$(subst .postmeta.h,,$@).raw,$(subst .postmeta.h,,$@))

$(BT_FW_NAME_HCD).gz.postmeta.h: $(BT_FW_NAME_HCD).gz.btpremeta.h
	$(call add_bt_meta,$(subst .gz.postmeta.h,,$@).raw,$(subst .postmeta.h,,$@))

$(BT_A1_FW_NAME).postmeta.h: $(BT_A1_FW_NAME).btpremeta.h
	$(call add_bt_meta,$(subst .postmeta.h,,$@).raw,$(subst .postmeta.h,,$@))

$(BT_A1_FW_NAME).gz.postmeta.h : $(BT_A1_FW_NAME).gz.btpremeta.h
	$(call add_bt_meta,$(subst .gz.postmeta.h,,$@).raw,$(subst .postmeta.h,,$@))

$(NVRAM_1DX).gz.postmeta.h : $(NVRAM_1DX).gz.premeta.h
	$(call add_nvram_meta,$(subst .gz.postmeta.h,,$@).raw,$(subst .postmeta.h,,$@))

$(NVRAM_PICO).gz.postmeta.h : $(NVRAM_PICO).gz.premeta.h
	$(call add_nvram_meta,$(subst .gz.postmeta.h,,$@).raw,$(subst .postmeta.h,,$@))

# Generate the combined wb firmware
$(WB_FIRMWARE_COMBINED).raw : $(WIFI_LATEST_FW_NAME).pad512 $(CLM_NAME)
	cat $^ > $@

# Generate the combined launch firmware
$(LAUNCH_FIRMWARE_COMBINED).raw : $(WIFI_LAUNCH_FW_NAME).pad512 $(CLM_NAME)
	cat $^ > $@

# Generate the combined a1 firmware
$(A1_FIRMWARE_COMBINED).raw : $(WIFI_A1_FW_NAME).pad512 $(CLM_A1_NAME).pad256
	cat $^ > $@

# nvram raw
wifi_nvram_%.raw : nvram_settings/wifi_nvram_%.raw
	cp $< $@
	touch $<

# Pad a file to 512 bytes
%.pad512 : %
	cp $< $@
	dd if=/dev/zero of=$@ bs=1 count=1 seek=$$(( ($$(stat -c %s $<) / 512) * 512 + 512 - 1))

# Pad a file to 256 bytes
%.pad256 : %
	cp $< $@
	dd if=/dev/zero of=$@ bs=1 count=1 seek=$$(( ($$(stat -c %s $<) / 256) * 256 + 256 - 1))

# Compress a "raw" file
%.gz : %.raw $(MINIZ_BIN_NAME)
	./$(MINIZ_BIN_NAME) $< $@

# Generate a compressed header file
%.gz.premeta.h : %.gz
	xxd -i $< | sed 's/^unsigned/static const unsigned/' | sed 's/\[\] = {/\[\] CYW43_RESOURCE_ATTRIBUTE = {/' > $@

# Generate a header file from a "raw" file
%.premeta.h : %.raw
	cp $< $(basename $<)
	xxd -i $(basename $<) | sed 's/^unsigned/static const unsigned/' | sed 's/\[\] = {/\[\] CYW43_RESOURCE_ATTRIBUTE = {/' > $@

# Generate a compressed header file
%.gz.btpremeta.h : %.gz
	xxd -i $< | sed 's/^unsigned/const unsigned/' | sed 's/\[\] = {/\[\] CYW43_RESOURCE_ATTRIBUTE = {/' > $@

# Generate a header file from a "raw" file
%.btpremeta.h : %.raw
	cp $< $(basename $<)
	xxd -i $(basename $<) | sed 's/^unsigned/const unsigned/' | sed 's/\[\] = {/\[\] CYW43_RESOURCE_ATTRIBUTE = {/' > $@

# Run the fw binary to get a raw binary dump of the bt firmware
$(BT_FW_NAME).raw : $(BT_FW_BIN_NAME)
	./$< | xxd -r -p > $@

$(BT_A1_FW_NAME).raw : a1_firmware/$(BT_A1_FW_NAME).raw
	cp $< $@

# Generate a program that outputs the bytes required for the bt firmware
$(BT_FW_BIN_NAME): bt_firmware/main.c bt_firmware/$(BT_FW_SRC)
	$(info *** generating: $@)
	$(CC) $^ -o $@

$(BT_FW_NAME_HCD).raw: $(BT_FW_READ_HCD_BIN_NAME) bt_firmware/$(BT_FW_HCD_IN)
	./$(BT_FW_READ_HCD_BIN_NAME) bt_firmware/$(BT_FW_HCD_IN) | xxd -r -p - > $@

# Converts a bt firmware in a binary HCD file into the format we expect
$(BT_FW_READ_HCD_BIN_NAME): bt_firmware/read_hcd.c
	$(info *** generating: $@)
	$(CC) $^ -o $@

tinf_objects = tinflate.o tinfgzip.o tinfzlib.o adler32.o crc32.o \
    defl_static.o genlz77.o

libtinf.a: $(tinf_objects)
	$(RM) $@
	$(AR) -frs $@ $^
	ranlib $@

%.o : $(PICO_SDK_PATH)/lib/uzlib/src/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(TGZIP_BIN_NAME): tgzip/tgzip.c libtinf.a
	$(CC) $^ -I $(PICO_SDK_PATH)/lib/uzlib/src -o $@

$(MINIZ_BIN_NAME): miniz/CMakeLists.txt
	mkdir -p build
	cmake -B build -S miniz
	make -C build

.PHONY : clean cleanjunk
cleanjunk :
	$(RM) \
		$(BT_FW_BIN_NAME) $(BT_FW_NAME) $(BT_FW_NAME_HCD) $(BT_A1_FW_NAME) $(BT_FW_READ_HCD_BIN_NAME) \
		$(shell find . -name "*.pad512") $(shell find . -name "*.pad256") \
		*.raw *.gz \
		*_combined \
		*.postmeta.h *.premeta.h *.btpremeta.h \
		*.o *.a $(TGZIP_BIN_NAME)
	$(RM) -fr build miniz/bin

clean: cleanjunk
	$(RM) \
		$(BT_FW_NAME).h $(BT_FW_NAME).gz.h  $(BT_A1_FW_NAME).h $(BT_A1_FW_NAME).gz.h \
		$(shell find . -name "*_combined.h") $(shell find . -name "*_combined.gz.h")
