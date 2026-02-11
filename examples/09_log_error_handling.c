/*
  Example 9: Custom Log and Error Handling
      This example demonstrates:
        1. Using a custom log handler to format warnings and errors.
        2. Handling config error types
        3. Using the minimum log level
*/

#include <inttypes.h>
#include <stdio.h>

#include "../clags.h"

/*
  A custom log handler, called by `clags_log` internally

  Arguments:
    - level  : the level of error (clags_log_level_t)
    - format : a printf-style format string
    - args   : the variadic arguments for the format printing
*/
void custom_log_handler(clags_log_level_t level, const char *format,
                        va_list args) {
  switch (level) {
  case Clags_Info:
    // should never occur since `min_log_level` is set to `Clags_Warning`
    printf("INFO: ");
    vprintf(format, args);
    printf("\n");
    break;
  case Clags_Warning:
    fprintf(stderr, "WARNING: ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    break;
  case Clags_Error:
    fprintf(stderr, "ERROR: ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    break;
  case Clags_ConfigWarning:
    fprintf(stderr, "CONFIG WARNING: ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    break;
  case Clags_ConfigError:
    fprintf(stderr, "CONFIG ERROR: ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    break;
  case Clags_NoLogs:
    break;
  default:
    clags_unreachable("Invalid log level!");
  }
}

int32_t lhs = 0;
int32_t rhs = 0;
bool help = false;

clags_arg_t args[] = {
    clags_positional(&lhs, "lhs", "left-hand side (integer)",
                     .value_type = Clags_Int32),
    clags_positional(&rhs, "rhs", "right-hand side (integer)",
                     .value_type = Clags_Int32),
    clags_flag_help(&help),
};

clags_config_t config = clags_config(
    args,
    .log_handler = custom_log_handler, // instruct clags to use the custom log
                                       // handler for logging
    .min_log_level = Clags_NoLogs, // do not print any logs of a lower level,
                                   // here: disable all logging
    .description = "Add two numbers with enhanced logging");

int main(int argc, char **argv) {
  const char *program_name = argv[0];

  clags_config_t *failed_config = clags_parse(argc, argv, &config);
  if (failed_config) {
    // re-enable logging
    failed_config->options.min_log_level = Clags_Info;

    // act based on error type
    if (failed_config->error == Clags_Error_InvalidValue) {
      clags_log(failed_config, Clags_Info,
                "Invalid number detected. Using defaults (lhs=0, rhs=0).");
      lhs = 0;
      rhs = 0;
    } else {
      // fatal: print detailed usage and exit
      clags_log(failed_config, Clags_Error, "error in '%s': %s",
                failed_config->name,
                clags_error_description(failed_config->error));
      clags_usage(program_name, failed_config);
      return 1;
    }
  }

  if (help) {
    clags_usage(program_name, &config);
    return 0;
  }

  printf("%" PRId32 " + %" PRId32 " = %" PRId32 "\n", lhs, rhs, lhs + rhs);
  return 0;
}
