/*
    Example 8: Image Processor
      This is a full example of most of clags's features and shows a realistic
   use case for clags. It implements the cli for an image processor that is
   capable of converting, resizing and tagging images.
*/

#include <stdint.h>
#include <stdio.h>

#define CLAGS_IMPLEMENTATION
#include "../clags.h"

/* Global arguments*/
clags_config_t *help = nullptr;
bool version = false;

// the options used for all configs
const clags_options_t options = {.ignore_prefix = "!", .list_terminator = "::"};

/* Convert subcommand config */

char *convert_input_file = nullptr;
char *convert_output_file = "out.png";
uint8_t convert_quality = 90;
bool convert_warnings = false;

clags_choice_t convert_choices[] = {
    {"PNG", ""},
    {"JPEG", ""},
    {"TIFF", ""},
};

clags_choices_t choices =
    clags_choices(convert_choices, .print_no_details = true);
clags_choice_t *convert_format = clags_choice_value(convert_choices, 0);

clags_arg_t convert_args[] = {
    clags_positional(&convert_input_file, "input_file", "the file to convert",
                     .value_type = Clags_File),
    clags_option('o', "output", &convert_output_file, "FILE",
                 "the file to output"),
    clags_option('f', "format", &convert_format, "FORMAT",
                 "the format which to convert to", .value_type = Clags_Choice,
                 .choices = &choices),
    clags_option('q', "quality", &convert_quality, "QUALITY",
                 "the quality of the convertion", .value_type = Clags_UInt8),
    clags_flag('w', "warnings", &convert_warnings, "print warnings"),
    clags_flag('v', "version", &version, "print the version", .exit = true),
    clags_flag_help_config(&help),
};

// this macro is an alternative to `clags_config`. You can use it if multiple
// configs share the same options.
clags_config_t convert_config =
    clags_config_with_options(convert_args, options);

/* Resize subcommand config */

char *resize_input_file = nullptr;
char *resize_output_file = "resized.png";
uint32_t resize_width = 0;
uint32_t resize_height = 0;
bool resize_keep_aspect = false;

clags_arg_t resize_args[] = {
    clags_positional(&resize_input_file, "input_file", "the file to resize",
                     .value_type = Clags_File),
    clags_option('o', "output", &resize_output_file, "FILE",
                 "the file to output"),
    clags_option('w', "width", &resize_width, "PIXELS",
                 "the width of the output image", .value_type = Clags_UInt32),
    clags_option('h', "height", &resize_height, "PIXELS",
                 "the height of the output image", .value_type = Clags_UInt32),
    clags_flag('k', "keep-aspect", &resize_keep_aspect,
               "keep the aspect ratio of the input image"),
    clags_flag('v', "version", &version, "print the version", .exit = true),
    clags_flag_help_config(&help),
};

clags_config_t resize_config = clags_config_with_options(resize_args, options);

/* Tag subcommand config */

clags_list_t tag_images = clags_list();
clags_list_t tag_values = clags_list();

bool tag_overwrite = false;

clags_choice_t tag_formats[] = {
    {"JSON", "Save tags in JSON format"},
    {"XML", "Save tags in XML format"},
    {"YAML", "Save tags in YAML format"},
};
clags_choices_t tag_choices = clags_choices(tag_formats);
clags_choice_t *tag_format = clags_choice_value(tag_formats, 0);

clags_arg_t tag_args[] = {
    clags_positional(&tag_images, "images", "list of images to tag",
                     .is_list = true, .value_type = Clags_File),
    clags_positional(&tag_values, "tags", "tags to apply to all images",
                     .is_list = true),
    clags_option('f', "format", &tag_format, "FORMAT", "format to save tags",
                 .value_type = Clags_Choice, .choices = &tag_choices),
    clags_flag('o', "overwrite", &tag_overwrite, "replace existing tags"),
    clags_flag('v', "version", &version, "print the version", .exit = true),
    clags_flag_help_config(&help),
};

clags_config_t tag_config = clags_config_with_options(tag_args, options);

/* Parent config */

typedef enum {
  Convert = 0,
  Resize,
  Tag,
} MySubcmds;

clags_subcmd_t subcommands[] = {
    [Convert] = {"convert", "convert an image to a different format",
                 &convert_config},
    [Resize] = {"resize", "resize an image ", &resize_config},
    [Tag] = {"tag", "tag multiple images", &tag_config},
};

clags_subcmds_t subcmds = clags_subcmds(subcommands);
clags_subcmd_t *selected_subcmd = nullptr;

clags_arg_t parent_args[] = {
    clags_positional(&selected_subcmd, "action", "the subcommand to run",
                     .value_type = Clags_Subcmd, .subcmds = &subcmds),
    clags_flag('v', "version", &version, "print the version", .exit = true),
    clags_flag_help_config(&help),
};

clags_config_t parent_config = clags_config_with_options(parent_args, options);

int main(int argc, char **argv) {
  int result = 0;
  const char *program_name = argv[0];

  // parsing
  clags_config_t *failed_config = clags_parse(argc, argv, &parent_config);
  if (failed_config) {
    // print usage for failed subcommand
    clags_usage(program_name, failed_config);
    clags_return_defer(1);
  }
  if (help) {
    // print help for the subcommand for which `--help` was provided
    clags_usage(argv[0], help);
    clags_return_defer(0);
  }
  if (version) {
    // this could have been set from any subcommand
    printf("%s - v.1.0.0\n", program_name);
    clags_return_defer(0);
  }

  // execute chosen subcommand
  MySubcmds subcmd = clags_subcmd_index(&subcmds, selected_subcmd);
  switch (subcmd) {
  case Convert: {
    printf("Convert subcommand selected:\n");
    printf("  Input file : %s\n", convert_input_file);
    printf("  Output file: %s\n", convert_output_file);
    printf("  Format     : %s\n", convert_format->value);
    printf("  Quality    : %" PRIu8 "\n", convert_quality);
    printf("  Warnings   : %s\n", convert_warnings ? "true" : "false");

    // convertion implementation goes here

  } break;
  case Resize: {
    printf("Resize subcommand selected:\n");
    printf("  Input file : %s\n", resize_input_file);
    printf("  Output file: %s\n", resize_output_file);
    printf("  Width      : %" PRIu32 "\n", resize_width);
    printf("  Height     : %" PRIu32 "\n", resize_height);
    printf("  Keep Aspect: %s\n", resize_keep_aspect ? "true" : "false");

    // resizing implementation goes here
  } break;
  case Tag: {
    printf("Tagging %zu images:\n", tag_images.count);
    for (size_t i = 0; i < tag_images.count; ++i) {
      printf("  Image: %s\n", clags_list_element(tag_images, char *, i));
    }
    printf("Tags to apply (%zu):\n", tag_values.count);
    for (size_t i = 0; i < tag_values.count; ++i) {
      printf("  %s\n", clags_list_element(tag_values, char *, i));
    }
    printf("Overwrite: %s\n", tag_overwrite ? "yes" : "no");
    printf("Format: %s\n", tag_format->value);

    // tagging implementation goes here

  } break;
  default: {
    fprintf(stderr, "[ERROR] Invalid subcommand selected: %p!\n",
            (void *)selected_subcmd);
    clags_return_defer(1);
  }
  }

defer:
  // free all the memory that clags might have allocated during parsing
  clags_list_free(&tag_images);
  clags_list_free(&tag_values);
  return result;
}
