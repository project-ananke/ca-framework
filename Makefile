CC := clang

TARGET_EXE := ananke
STYGATORE_EXE := stygatore

BIN_DIR := bin
BUILD_DIR := build
SRC_DIR := engine
STYGATORE_GEN_DIR := engine/generated

SRCS := $(shell find . -name "*.c")

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

STYGATORE_SRCS := $(wildcard engine/generated/*.styx)
STYGATORE_HEADERS := $(STYGATORE_SRCS:.styx=.h)

CFLAGS := -g -Iext/lua544 `sdl2-config --cflags` -MMD -MP -Wno-incompatible-pointer-types-discards-qualifiers
LDFLAGS := `sdl2-config --libs`

$(BIN_DIR)/$(TARGET_EXE): $(BIN_DIR)/stygatore $(STYGATORE_HEADERS) $(OBJS)
	@mkdir -p $(dir $@)
	@echo "  LINK	$@"
	@$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BIN_DIR)/$(STYGATORE_EXE): $(STYGATORE_DEPS)
	@mkdir -p $(dir $@)
	@echo "  GO	$@"
	@pushd engine/stygatore > /dev/null && go build && popd > /dev/null
	@mv engine/stygatore/stygatore $@

%.h: %.styx
	@echo "  STYX	$<"
	@bin/stygatore $< > /dev/null

$(BUILD_DIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	@echo "  CC	$<"
	@$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean run
clean:
	@rm -rf $(BUILD_DIR)/* $(BIN_DIR)/* $(STYGATORE_GEN_DIR)/*.h

run:
	@./bin/ananke
	
-include $(DEPS)
