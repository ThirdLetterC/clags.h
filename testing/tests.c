#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define CLAGS_IMPLEMENTATION
#include "../clags.h"

clags_options_t global_options = {
    .min_log_level = Clags_NoLogs,
};

// 1. Integer option
void test_int_option() {
  int num = 0;

  clags_config_t config = {
      .args = (clags_arg_t[]){{.type = Clags_Option,
                               .opt = {.long_flag = "num",
                                       .value_type = Clags_Int32,
                                       .variable = &num}}},
      .args_count = 1,
      .options = global_options,
  };

  char *argv[] = {"prog", "--num", "123"};
  int argc = 3;

  clags_config_t *parse_result = clags_parse(argc, argv, &config);
  assert(parse_result == nullptr);
  assert(num == 123);
  assert(config.error == Clags_Error_Ok);
}

// 2. Float option
void test_float_option() {
  double fval = 0.0f;

  clags_config_t config = {
      .args = (clags_arg_t[]){{.type = Clags_Option,
                               .opt = {.long_flag = "value",
                                       .value_type = Clags_Double,
                                       .variable = &fval}}},
      .args_count = 1,
      .options = global_options,
  };

  char *argv[] = {"prog", "--value", "3.14"};
  int argc = 3;

  clags_config_t *parse_result = clags_parse(argc, argv, &config);
  assert(parse_result == nullptr);
  assert(fabs(fval - 3.14) < 1e-6);
  assert(config.error == Clags_Error_Ok);
}

// 3. Boolean option
void test_bool_option() {
  bool flag = false;

  clags_config_t config = {
      .args = (clags_arg_t[]){{.type = Clags_Option,
                               .opt = {.long_flag = "enable",
                                       .value_type = Clags_Bool,
                                       .variable = &flag}}},
      .args_count = 1,
      .options = global_options,
  };

  char *argv[] = {"prog", "--enable", "yes"};
  int argc = 3;

  clags_config_t *parse_result = clags_parse(argc, argv, &config);
  assert(parse_result == nullptr);
  assert(flag == true);
  assert(config.error == Clags_Error_Ok);
}

// 4. Short flag
void test_short_flag() {
  bool verbose = false;

  clags_config_t config = {
      .args = (clags_arg_t[]){{.type = Clags_Flag,
                               .flag = {.short_flag = 'v',
                                        .type = Clags_BoolFlag,
                                        .variable = &verbose}}},
      .args_count = 1,
      .options = global_options,
  };

  char *argv[] = {"prog", "-v"};
  int argc = 2;

  clags_config_t *parse_result = clags_parse(argc, argv, &config);
  assert(parse_result == nullptr);
  assert(verbose == true);
  assert(config.error == Clags_Error_Ok);
}

// 5. Positional argument
void test_positional() {
  char *file = nullptr;

  clags_config_t config = {
      .args = (clags_arg_t[]){{.type = Clags_Positional,
                               .pos = {.arg_name = "file",
                                       .value_type = Clags_String,
                                       .variable = &file,
                                       .optional = false}}},
      .args_count = 1,
      .options = global_options,
  };

  char *argv[] = {"prog", "input.txt"};
  int argc = 2;

  clags_config_t *parse_result = clags_parse(argc, argv, &config);
  assert(parse_result == nullptr);
  assert(file && strcmp(file, "input.txt") == 0);
  assert(config.error == Clags_Error_Ok);
}

// 6. Positional list
void test_positional_list() {
  clags_list_t files = clags_list();

  clags_config_t config = {
      .args = (clags_arg_t[]){{.type = Clags_Positional,
                               .pos = {.arg_name = "files",
                                       .value_type = Clags_String,
                                       .variable = &files,
                                       .is_list = true}}},
      .args_count = 1,
      .options = global_options,
  };

  char *argv[] = {"prog", "a.txt", "b.txt", "c.txt"};
  int argc = 4;

  clags_config_t *parse_result = clags_parse(argc, argv, &config);
  assert(parse_result == nullptr);
  assert(files.count == 3);
  assert(strcmp(((char **)files.items)[0], "a.txt") == 0);
  assert(strcmp(((char **)files.items)[2], "c.txt") == 0);
  assert(config.error == Clags_Error_Ok);

  clags_config_free(&config);
}

// 7. Invalid value
void test_invalid_value() {
  int num = 0;

  clags_config_t config = {
      .args = (clags_arg_t[]){{.type = Clags_Option,
                               .opt = {.long_flag = "num",
                                       .value_type = Clags_Int32,
                                       .variable = &num}}},
      .args_count = 1,
      .options = global_options,
  };

  char *argv[] = {"prog", "--num", "abc"};
  int argc = 3;

  clags_config_t *parse_result = clags_parse(argc, argv, &config);
  assert(parse_result == &config);
  assert(config.error == Clags_Error_InvalidValue);
}

// 8. Subcommand
void test_subcommand() {
  clags_config_t init_config = {0};
  clags_subcmd_t subcmds[] = {{.name = "init", .config = &init_config}};
  clags_subcmds_t list = {.items = subcmds, .count = 1};

  clags_config_t config = {
      .args =
          (clags_arg_t[]){{.type = Clags_Positional,
                           .pos = {.arg_name = "command",
                                   .value_type = Clags_Subcmd,
                                   .subcmds = &list,
                                   .variable = &(clags_subcmd_t *){nullptr}}}},
      .args_count = 1,
      .options = global_options,
  };

  char *argv[] = {"prog", "init"};
  int argc = 2;

  clags_config_t *parse_result = clags_parse(argc, argv, &config);
  assert(parse_result == nullptr);
  assert(config.error == Clags_Error_Ok);
}

int main() {
  test_int_option();
  printf("- Test 'int option' passed!\n");
  test_float_option();
  printf("- Test 'float option' passed!\n");
  test_bool_option();
  printf("- Test 'bool option' passed!\n");
  test_short_flag();
  printf("- Test 'short flag' passed!\n");
  test_positional();
  printf("- Test 'positional' passed!\n");
  test_positional_list();
  printf("- Test 'positional list' passed!\n");
  test_invalid_value();
  printf("- Test 'invalid value' passed!\n");
  test_subcommand();
  printf("- Test 'subcommand' passed!\n");

  printf("\nAll tests passed!\n");
  return 0;
}
