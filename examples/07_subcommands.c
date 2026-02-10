/*
  Example 7: Subcommands
      This example demonstrates how to define subcommands (like 'git commit' or
  'git push'). Each subcommand has its own argument parser configuration.
*/

#include <stdio.h>

#define CLAGS_IMPLEMENTATION
#include "../clags.h"

// Global help flag
// since the help flag can be set from multiple subcommands, we can track the
// corresponding config here
clags_config_t *help = nullptr;

// This flag tracks how many times it was encountered
size_t verbosity = 0;

// --- COPY subcommand ---
char *copy_source = nullptr;
char *copy_dest = nullptr;

clags_arg_t copy_args[] = {
    clags_positional(&copy_source, "source", "Source file path",
                     .value_type = Clags_File),
    clags_positional(&copy_dest, "dest", "Destination file path"),

    // With the `.type` field, we specify how the flag stores its result.
    // A full list of available storage types can be found in the
    // `clags_flag_type_t` enum. The default is `Clags_BoolFlag`, a normal
    // boolean flag.

    // `.type = Clags_CountFlag` instructs the parser to track how many times
    // this flag was encountered. The associated variable must be of type
    // `size_t`.
    clags_flag('v', "verbose", &verbosity, "increase verbosity",
               .type = Clags_CountFlag),

    // `.type = Clags_ConfigFlag` instructs the parser to store a pointer to
    // the `clags_config_t` of the (sub)command in which the flag was
    // encountered. The associated variable must be of type `clags_config_t *`.
    clags_flag('h', "help", &help, "print this help dialog", .exit = true,
               .type = Clags_ConfigFlag),
};

clags_config_t copy_config = clags_config(copy_args);

// --- DELETE subcommand ---
char *delete_target = nullptr;
bool delete_force = false;

clags_arg_t delete_args[] = {
    clags_positional(&delete_target, "target", "Target file to delete",
                     .value_type = Clags_File),
    clags_flag('f', "force", &delete_force, "Force deletion"),

    clags_flag('v', "verbose", &verbosity, "increase verbosity",
               .type = Clags_CountFlag),
    // This is a built-in shorthand for the previously defined help flag
    clags_flag_help_config(&help)};

clags_config_t delete_config = clags_config(delete_args);

// --- Subcommands ---
// This links the previously created subcommand configs to their respective
// arguments
clags_subcmd_t subcmds[] = {
    {"copy", "Copy a file from source to destination", &copy_config},
    {"delete", "Delete a file", &delete_config}};

clags_subcmds_t my_subcmds = clags_subcmds(subcmds);

// Define a variable that will hold a pointer to the selected `clags_subcmd_t`
clags_subcmd_t *selected_subcmd = nullptr;

// --- Main CLI config ---
clags_arg_t main_args[] = {
    // With `.subcmds` you set the subcommand parsing verifier
    clags_positional(&selected_subcmd, "command", "Subcommand to run",
                     .value_type = Clags_Subcmd, .subcmds = &my_subcmds),
    clags_flag('v', "verbose", &verbosity, "increase verbosity",
               .type = Clags_CountFlag),
    clags_flag_help_config(&help)};

clags_config_t main_config = clags_config(main_args);

int main(int argc, char **argv) {

  // `clags_parse` returns the config that failed.
  // In basic examples this will always be the config provided to `clags_parse`
  // or `nullptr`. But since we have nested configs here, this can be used to
  // determine which subcommand failed
  clags_config_t *failed_config = clags_parse(argc, argv, &main_config);
  if (failed_config) {
    // print the usage for the failed subcommand
    clags_usage(argv[0], failed_config);
    return 1;
  }

  if (help) {
    // In this example, the `help` flag is allowed to be set by all subcommands.
    // Therefore, we call `clags_usage` based on the selected subcommand, whose
    // config is stored in the `help` variable
    clags_usage(argv[0], help);
    return 0;
  }

  printf("Verbosity is %zu.\n", verbosity);

  // Act based on which subcommand was selected
  // This function returns the index of the subcommand within the definition
  // above. Pointer comparision can of course also be used.
  int subcmd_index = clags_subcmd_index(&my_subcmds, selected_subcmd);
  switch (subcmd_index) {
  case 0: {
    // copy subcommand
    printf("Copying file '%s' to '%s'\n", copy_source, copy_dest);
    // actual copy logic goes here
  } break;
  case 1: {
    // delete subcommand
    printf("Deleting file '%s'%s\n", delete_target,
           delete_force ? " (force)" : "");
    // actual delete logic goes here
  } break;
  default: {
    fprintf(stderr, "[ERROR] Invalid subcommand: %s\n", selected_subcmd->name);
  }
  }

  return 0;
}
