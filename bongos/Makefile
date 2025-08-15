SRC = bongos.ino
BUILD_DIR = built

# In MHz
# To see options:
# arduino-cli compile -b teensy:avr:teensy31 --show-properties | rg '^menu\.speed\.\d{1,3}=' | cut -d = -f 2 | sort -nr
F_CPU = 72

# arduino-cli compile -b teensy:avr:teensy31 --show-properties | rg '^menu\.usb\.\w*?=' | cut -d . -f 3 | sort | awk -F '=' '{printf "%-40s = %s\n", $2, $1}'
USB_TYPE = serialhid

# arduino-cli compile -b teensy:avr:teensy31 --show-properties | rg '^menu\.opt\.\w*?=' | cut -d . -f 3 | sort | awk -F '=' '{printf "%-40s = %s\n", $2, $1}
OPTIMIZATION_LEVEL = o3std

all: compile flash

compile: $(SRC)
	arduino-cli compile -b teensy:avr:teensy31:speed=$(F_CPU),usb=$(USB_TYPE),opt=$(OPTIMIZATION_LEVEL) $< --output-dir $(BUILD_DIR)

flash: $(BUILD_DIR)/$(SRC).hex
	teensy_loader_cli --mcu=mk20dx256 -w -v $<

.PHONY: all compile flash
