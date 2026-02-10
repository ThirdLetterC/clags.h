/*
  Example 3.2: Multiple Lists
     This example shows how to work with multiple lists.
     To terminate a list, use the custom `.list_terminator` feature
*/

#include <inttypes.h>
#include <stdio.h>

#define CLAGS_IMPLEMENTATION
#include "../clags.h"

clags_list_t string_list = clags_list();

// Lists can also be typed
clags_list_t int_list = clags_int32_list();
clags_list_t file_list = clags_file_list();
clags_list_t extra_list = clags_string_list();

bool help = false;

clags_arg_t args[] = {
    // You can simply combine the known `.value_type` and `.is_list` fields to
    // achieve typed lists
    clags_positional(&string_list, "strings", "a list of strings",
                     .is_list = true),
    clags_positional(&int_list, "ints", "a list of integers",
                     .value_type = Clags_Int32, .is_list = true),
    // Positional arguments may also be optional, so the parser does not throw
    // an error if no argument is provided
    clags_positional(&extra_list, "extras", "extra arguments", .optional = true,
                     .is_list = true),

    // Options can also be lists
    // Every time the user provides the flag together with an argument that
    // argument is added to the list
    clags_option('f', "file", &file_list, "FILE", "a list of files",
                 .value_type = Clags_File, .is_list = true),
    clags_flag_help(&help),
};

// Having multiple lists or, in fact, any other required arguments directly
// after a list, requires for a delimeter that terminates the list. When
// `.duplicate_strings` is true, clags duplicates all strings. This is necessary
// if the original argument strings may go out of scope before the parsed values
// are used.
clags_config_t config = clags_config(
    args, .list_terminator = "::", .ignore_prefix = "!",
    .allow_option_parsing_toggle = true, .duplicate_strings = true);

int main(int argc, char **argv) {
  int result = 0;
  const char *program_name = argv[0];

  if (clags_parse(argc, argv, &config) != nullptr) {
    clags_usage(program_name, &config);
    // this makes working with lists way more convenient
    clags_return_defer(1);
  }
  if (help) {
    clags_usage(program_name, &config);
    clags_return_defer(0);
  }

  printf("Strings (%zu):\n", string_list.count);
  for (size_t i = 0; i < string_list.count; ++i) {
    printf("  %zu: %s\n", i, clags_list_element(string_list, char *, i));
  }
  printf("\nInts (%zu):\n", int_list.count);
  for (size_t i = 0; i < int_list.count; ++i) {
    printf("  %zu: %" PRId32 "\n", i, clags_list_element(int_list, int32_t, i));
  }
  printf("\nFiles (%zu):\n", file_list.count);
  for (size_t i = 0; i < file_list.count; ++i) {
    printf("  %zu: %s\n", i, clags_list_element(file_list, char *, i));
  }
  printf("\nExtras (%zu):\n", extra_list.count);
  for (size_t i = 0; i < extra_list.count; ++i) {
    printf("  %zu: %s\n", i, clags_list_element(extra_list, char *, i));
  }

defer:
#if 1
  // manually free each list explicitly
  clags_list_free(&string_list);
  clags_list_free(&int_list);
  clags_list_free(&extra_list);
  clags_list_free(&file_list);

  // the config stores all the duplicated strings. You can free them like this
  clags_config_free_allocs(&config);
#else
  // alternatively, all this can be done automatically
  clags_config_free(&config);
#endif
  return result;
}
