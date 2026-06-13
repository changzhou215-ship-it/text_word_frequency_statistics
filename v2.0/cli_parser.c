/* cli_parser.c — 命令行参数解析实现
 * 功能：解析 main 的 argc/argv，填充 CliConfig 结构体
 * v1.1 新增模块
 * [v2.0 扩展] 新增 -c -s --sort --html 四个 flag
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cli_parser.h"


void print_usage(const char *program_name) {
    printf("Usage: %s -f <file> [options]\n\n", program_name);
    printf("Options:\n");
    printf("  -f <file>              Input text file (required)\n");
    printf("  -e <word1> <word2>...  Words to exclude from analysis\n");
    printf("  -t <N>                 Show top N words (default: all)\n");
    printf("  --min-length <N>       Minimum word length (default: 0)\n");
    printf("  --max-length <N>       Maximum word length (default: unlimited)\n");
    printf("  -p                     Show percentage for each word\n");
    printf("  -o <file>              Write output to file\n");
    /* [v2.0 新增] */
    printf("  -c                     Case-sensitive counting (default: off)\n");
    printf("  -s                     Enable built-in stopwords filter (~120 words)\n");
    printf("  --sort <freq|alpha>    Sort by frequency (default) or alphabetically\n");
    printf("  --html                 Output as HTML report instead of text\n");
    printf("\n");
    printf("Example: %s -f sample.txt -t 10 -p -s -o result.html --html\n", program_name);
}


CliConfig parse_cli(int argc, char *argv[]) {
    CliConfig cfg;

    /* 默认值 */
    cfg.file_path[0] = '\0';
    cfg.output_path[0] = '\0';
    cfg.exclude_count = 0;
    cfg.top_n = 0;
    cfg.min_length = 0;
    cfg.max_length = 0;
    cfg.show_percentage = 0;
    cfg.has_error = 0;
    /* [v2.0] 新字段默认值 */
    cfg.case_sensitive = 0;
    cfg.use_stopwords = 0;
    cfg.sort_mode = 0;      /* 0 = freq */
    cfg.html_output = 0;

    /* 逐参数解析 */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0) {
            if (i + 1 < argc) {
                strcpy(cfg.file_path, argv[++i]);
            } else {
                fprintf(stderr, "Error: -f requires a file path.\n");
                cfg.has_error = 1;
                return cfg;
            }
        }
        else if (strcmp(argv[i], "-e") == 0) {
            while (i + 1 < argc && argv[i + 1][0] != '-'
                   && cfg.exclude_count < MAX_EXCLUDE_WORDS) {
                strcpy(cfg.exclude_words[cfg.exclude_count], argv[++i]);
                cfg.exclude_count++;
            }
        }
        else if (strcmp(argv[i], "-t") == 0) {
            if (i + 1 < argc) {
                cfg.top_n = atoi(argv[++i]);
                if (cfg.top_n < 0) cfg.top_n = 0;
            } else {
                fprintf(stderr, "Error: -t requires a number.\n");
                cfg.has_error = 1;
                return cfg;
            }
        }
        else if (strcmp(argv[i], "--min-length") == 0) {
            if (i + 1 < argc) {
                cfg.min_length = atoi(argv[++i]);
            } else {
                fprintf(stderr, "Error: --min-length requires a number.\n");
                cfg.has_error = 1;
                return cfg;
            }
        }
        else if (strcmp(argv[i], "--max-length") == 0) {
            if (i + 1 < argc) {
                cfg.max_length = atoi(argv[++i]);
            } else {
                fprintf(stderr, "Error: --max-length requires a number.\n");
                cfg.has_error = 1;
                return cfg;
            }
        }
        else if (strcmp(argv[i], "-p") == 0) {
            cfg.show_percentage = 1;
        }
        else if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                strcpy(cfg.output_path, argv[++i]);
            } else {
                fprintf(stderr, "Error: -o requires an output file path.\n");
                cfg.has_error = 1;
                return cfg;
            }
        }
        /* === [v2.0 新增] === */
        else if (strcmp(argv[i], "-c") == 0) {
            cfg.case_sensitive = 1;
        }
        else if (strcmp(argv[i], "-s") == 0) {
            cfg.use_stopwords = 1;
        }
        else if (strcmp(argv[i], "--sort") == 0) {
            if (i + 1 < argc) {
                if (strcmp(argv[i + 1], "alpha") == 0)
                    cfg.sort_mode = 1;
                else
                    cfg.sort_mode = 0;  /* "freq" 或非法值 → 默认降序 */
                i++;
            } else {
                fprintf(stderr, "Error: --sort requires 'freq' or 'alpha'.\n");
                cfg.has_error = 1;
                return cfg;
            }
        }
        else if (strcmp(argv[i], "--html") == 0) {
            cfg.html_output = 1;
        }
    }

    /* -f 是必需的 */
    if (cfg.file_path[0] == '\0') {
        fprintf(stderr, "Error: -f <file> is required.\n");
        cfg.has_error = 1;
    }

    return cfg;
}
