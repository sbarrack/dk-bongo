SRC = bongos.ino
BUILD_DIR = built

all: compile flash

compile: $(SRC)
	arduino-cli compile -b teensy:avr:teensy31 $< --output-dir $(BUILD_DIR)

flash: $(BUILD_DIR)/$(SRC).hex
	teensy_loader_cli --mcu=mk20dx256 -w -v $<

.PHONY: all compile flash
