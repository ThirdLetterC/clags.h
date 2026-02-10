set shell := ["bash", "-eu", "-o", "pipefail", "-c"]

cc := env_var_or_default("CC", "cc")
std := env_var_or_default("STD", "-std=c2x")
warn := env_var_or_default("WARN", "-Wall -Wextra -Wpedantic -Werror")
san := env_var_or_default("SAN", "-fsanitize=address,undefined,leak -fno-omit-frame-pointer")
base_cflags := std + " " + warn
examples := "examples/01_basic examples/02_types examples/03_1_lists examples/03_2_multi-lists examples/04_choices examples/05_paths examples/06_custom examples/07_subcommands examples/08_image_processor examples/09_log_error_handling"

default: all

all: examples-build tests-build

examples-build:
    @for src in examples/*.c; do exe="${src%.c}"; {{cc}} {{base_cflags}} -o "$exe" "$src"; done

examples-debug:
    @for src in examples/*.c; do exe="${src%.c}"; {{cc}} {{base_cflags}} -g3 {{san}} -o "$exe" "$src" {{san}}; done

tests-build:
    {{cc}} {{base_cflags}} -o testing/tests testing/tests.c -lm

tests-debug:
    {{cc}} {{base_cflags}} -g3 {{san}} -o testing/tests testing/tests.c -lm {{san}}

test: tests-build
    ./testing/tests

test-debug: tests-debug
    ./testing/tests

zig-all:
    zig build

zig-examples:
    zig build examples

zig-tests:
    zig build tests

zig-test:
    zig build test

format:
    clang-format -i clags.h examples/*.c testing/tests.c

clean:
    rm -f {{examples}} testing/tests

clean-zig:
    rm -rf .zig-cache zig-out
