.PHONY: all distclean clean debug
export CC:=clang

export BUILD_DIR=$(PWD)/build
export SRC_DIR=$(PWD)/Source
export INC_DIR=$(PWD)/Include

export build_libchat=false
ifeq ($(build_libchat),true)
export LIB_CFLAGS=--shared -I$(INC_DIR)/ChatModel -I$(INC_DIR)/MessageInput -I$(INC_DIR)/MessageOutput -I$(INC_DIR)/utils -g -O0 -Wall
export OBJ_CFLAGS=-fPIC -I$(INC_DIR)/ChatModel -I$(INC_DIR)/MessageInput -I$(INC_DIR)/MessageOutput -I$(INC_DIR)/utils -g -O0 -Wall
export TARGET := $(BUILD_DIR)/libchat.so
else
export CFLAGS=-I$(INC_DIR)/ChatModel -I$(INC_DIR)/MessageInput -I$(INC_DIR)/MessageOutput -I$(INC_DIR)/utils -g -O0 -Wall
export TARGET := $(BUILD_DIR)/test
endif

all: $(TARGET)

$(TARGET):
	[ -d $(BUILD_DIR) ] || mkdir $(BUILD_DIR)
	$(MAKE) -C $(SRC_DIR)

libchat:
	$(MAKE) build_libchat=true
	$(MAKE) all

clean:
	$(MAKE) -C $(SRC_DIR) clean

distclean:
	rm -rf $(BUILD_DIR)/*