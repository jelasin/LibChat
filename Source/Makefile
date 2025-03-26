.PHONY: all clean
SUB_DIRS := ChatModel MessageInput MessageOutput utils

ifeq ($(build_libchat),true)
all: $(BUILD_DIR)/libchat.so
	$(CC) $(LIB_CFLAGS) -o $(TARGET) $(foreach dir,$(SUB_DIRS),$(wildcard $(BUILD_DIR)/$(dir)/*.o)) -lcurl

$(BUILD_DIR)/libchat.so:
	$(foreach dir,$(SUB_DIRS),$(MAKE) -C $(SRC_DIR)/$(dir) all;)

clean:
	$(foreach dir,$(SUB_DIRS),$(MAKE) -C $(SRC_DIR)/$(dir) clean;)
else
all: $(BUILD_DIR)/main.o
	$(CC) $(CFLAGS) -o $(TARGET) $(BUILD_DIR)/main.o $(foreach dir,$(SUB_DIRS),$(wildcard $(BUILD_DIR)/$(dir)/*.o)) -lcurl

$(BUILD_DIR)/main.o: main.c
	$(foreach dir,$(SUB_DIRS),$(MAKE) -C $(SRC_DIR)/$(dir) all;)
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)/main.o main.c

clean:
	$(foreach dir,$(SUB_DIRS),$(MAKE) -C $(SRC_DIR)/$(dir) clean;)
	@rm -f $(BUILD_DIR)/main.o
endif