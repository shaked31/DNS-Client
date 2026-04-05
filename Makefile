CC = gcc
CFLAGS = -std=c17 -Wall -Wextra -Wpedantic -Iinclude -INetwork -IUtils
LDFLAGS = -lws2_32
BUILD_DIR = build
SRCS = main.c Network/dns.c Network/NetworkManager.c Utils/Serializor.c
OBJECTS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(SRCS)))
HEADERS = include/dns.h include/packet.h Network/NetworkManager.h Utils/Serializor.h
TARGET = $(BUILD_DIR)/DNS_Client.exe
VPATH = .:Network:Utils

.PHONY: all clean
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.c $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)

clean:
	if exist $(BUILD_DIR) del /f /q /s $(BUILD_DIR)\*
