/*
  Example 6: Custom Verifiers
      This example shows how to use custom verification functions.
*/

#include <stdio.h>

#include "../clags.h"

// A custom verification function of type `clags_custom_verify_func_t`
// Arguments provided by the parser:
//   - `config`   : the config currently being parsed, mostly useful for logging
//   - `arg_name` : the name of the argument currently being parsed, useful for
//   logging
//   - `arg`      : the argument currently being parsed
//   - `variable` : the pointer of the variable to set
bool verify_lower_case(clags_config_t *config, const char *arg_name,
                       const char *arg, void *variable) {
  // Only accept lower case strings
  if (arg && islower(*arg)) {
    // Set the variable to the current arg since we are operating on strings
    // here Note: `clags_config_duplicate_string` does not duplicate the string
    // in this example, since the `duplicate_strings` option in the config is
    // not set.
    //       But using the function nethertheless makes it easy to change this
    //       behaviour without having to edit all your custom verifiers.
    if (variable)
      *(char **)variable = clags_config_duplicate_string(config, arg);
    // `arg` matches criteria, return success
    return true;
  }
  // `arg` does not match criteria, return failure which will also fail the
  // parser
  clags_log(config, Clags_Error,
            "String is not lower case for argument '%s': '%s'!\n", arg_name,
            arg);
  return false;
}

// For custom list it is mandatory to provide the size of the stored values
clags_list_t list = clags_custom_list(sizeof(char *));
bool help = false;

clags_arg_t args[] = {
    // Set `.verify` to the custom verfication function
    clags_positional(&list, "values", "lower case strings",
                     .value_type = Clags_Custom, .verify = verify_lower_case,
                     .is_list = true),
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
  printf("The lower case inputs are:\n");
  for (size_t i = 0; i < list.count; ++i) {
    printf("%3zu: '%s'\n", i, clags_list_element(list, char *, i));
  }
  clags_list_free(&list);
  return 0;
}
