/*
  Example 2: Types
      This example demonstrates how to add type verification to arguments
*/

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#define CLAGS_IMPLEMENTATION
#include "../clags.h"

char *input_file = nullptr;
char *output_file = "a.out";
uint8_t quality = 100;
bool help = false;
bool warnings = false;
bool version = false;

clags_arg_t args[] = {
    clags_positional(&input_file, "input_file", "the file to read"),
    clags_option('o', "output", &output_file, "FILE", "the file to write"),

    // to add type verification to an argument, simply set the `value_type`
    // field for a list of all available types, see `clags__types` in clags.h
    // Note: it is the user's responsibility to provide a variable pointer
    // matching the type specified
    //       so here, &quality is expected to be uint8_t*
    clags_option('q', "quality", &quality, "NUM",
                 "the quality of the output image", .value_type = Clags_UInt8),
    clags_flag('w', "warnings", &warnings, "print warnings"),

    // this is how you create a flag that exits the parsing on occurrence, just
    // like with `clags_flag_help`
    clags_flag('v', "version", &version, "print the version", .exit = true),
    clags_flag_help(&help),
};

clags_config_t config = clags_config(args);

int main(int argc, char **argv) {
  const char *program_name = argv[0];
  if (clags_parse(argc, argv, &config) != nullptr) {
    clags_usage(program_name, &config);
    return 1;
  }
  if (help) {
    clags_usage(program_name, &config);
    return 0;
  }
  if (version) {
    printf("02_types: v1.0.0\n");
    return 0;
  }
  printf("Reading: '%s', Writing: '%s', Quality: %" PRIu8 ", Warnings: %d\n",
         input_file, output_file, quality, warnings);
  return 0;
}
