.PHONY: all clean

SRCS := $(wildcard *.c)
OBJS := $(SRCS:%.c=$(BUILD_DIR)/utils/%.o)

all: $(OBJS)

$(BUILD_DIR)/utils/%.o: %.c
	@[ -d $(BUILD_DIR)/utils ] || mkdir -p $(BUILD_DIR)/utils
ifeq ($(build_libchat),true)
	$(CC) $(OBJ_CFLAGS) -c $< -o $@
else
	$(CC) $(CFLAGS) -c $< -o $@
endif

clean:
	@rm -rf $(BUILD_DIR)/utils