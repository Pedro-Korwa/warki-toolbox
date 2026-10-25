# Makefile untuk Warki Toolbox

BUILD_DIR = build

all: build

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

configure: $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake ..

build: configure
	cd $(BUILD_DIR) && make -j$$(nproc)

run: build
	./$(BUILD_DIR)/WarkiToolbox

clean:
	rm -rf $(BUILD_DIR)