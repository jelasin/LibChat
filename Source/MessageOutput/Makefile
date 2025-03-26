.PHONY: all clean

SRCS := $(wildcard *.c)
OBJS := $(SRCS:%.c=$(BUILD_DIR)/MessageOutput/%.o)

all: $(OBJS)

$(BUILD_DIR)/MessageOutput/%.o: %.c
	@[ -d $(BUILD_DIR)/MessageOutput ] || mkdir -p $(BUILD_DIR)/MessageOutput
ifeq ($(build_libchat),true)
	$(CC) $(OBJ_CFLAGS) -c $< -o $@
else
	$(CC) $(CFLAGS) -c $< -o $@
endif

clean:
	@rm -rf $(BUILD_DIR)/MessageOutput