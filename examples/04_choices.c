/*
  Example 4: Choices
      Choices allow the user to define a set of allowed literal strings (like an
  enum) which the parser will accept. Invalid strings will trigger an error.
*/

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include "clags/clags.h"

char *input = nullptr;
char *algorithm = nullptr;
char *output = "output.pdf";
bool warnings = false;
bool help = false;

uint8_t quality = 0;

// Define an array of possible choices
clags_choice_t choice_values[] = {{"LIFO", "last-in first-out"},
                                  {"FIFO", "first-in first-out"},
                                  {"RANDOM", "random order"}};

// Create a wrapper for the choices
// Choices are case-sensitive by default, with `.case_insensitive=true` you can
// disable that behaviour
clags_choices_t choices =
    clags_choices(choice_values, .case_insensitive = true);
// Create variable to hold the pointer to the chosen `clags_choice_t`
// This sets the default value to the first `clags_choice_t` in `choices`
// This expands to `clags_choice_t *choice = &choice_values[0];`
clags_choice_t *choice = clags_choice_value(choice_values, 0);

clags_arg_t args[] = {
    clags_positional(&input, "input_file", "the input file"),

    clags_option('o', "output", &output, "FILE", "the output file"),

    // With `.choices` you set the `clags_choices_t` wrapper as the verifier for
    // choice parsing
    clags_option('a', "algorithm", &choice, "ALG", "the algorithm to use",
                 .value_type = Clags_Choice, .choices = &choices),
    clags_option('q', "quality", &quality, "LEVEL", "the sample quality",
                 .value_type = Clags_UInt8),

    clags_flag('w', nullptr, &warnings, "print warnings"),
    clags_flag_help(&help),
};

clags_config_t config = clags_config(args);

int main(int argc, char **argv) {
  if (clags_parse(argc, argv, &config) != nullptr) {
    clags_usage(argv[0], &config);
    return 1;
  }
  if (help) {
    clags_usage(argv[0], &config);
    return 0;
  }
  printf("input: %s\n", input);
  printf("algorithm: %s or choice no. %d\n", choice->value,
         clags_choice_index(&choices, choice));
  printf("quality: %" PRId8 "\n", quality);
  printf("output: %s\n", output);
  return 0;
}
