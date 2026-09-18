CC ?= cc
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -Wpedantic

BUILD_DIR := build
RUNNER := $(BUILD_DIR)/signal-runner
FIXTURE := $(BUILD_DIR)/signal-fixture

.PHONY: all clean smoke-test

all: $(RUNNER) $(FIXTURE)

$(RUNNER): src/signal_runner.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(FIXTURE): tests/signal_fixture.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR):
	mkdir -p $@

smoke-test: all
	$(RUNNER) $(FIXTURE) normal
	$(RUNNER) $(FIXTURE) terminate

clean:
	rm -rf $(BUILD_DIR)
