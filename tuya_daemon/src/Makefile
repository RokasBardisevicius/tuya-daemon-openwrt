TARGET = tuyad
SRC_DIR = .
CPPFLAGS = -I./include -I$(STAGING_DIR)/usr/include/cjson
BUILD_DIR = build
LDFLAGS = -L./tuya-iot-core-sdk/build/lib -llink_core -lmiddleware_implementation -lplatform_port -lutils_modules -ljson-c -largp -Wl,-rpath,./tuya-iot-core-sdk/build/lib -ggdb3 -lubus -lubox -lblobmsg_json -luci

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
