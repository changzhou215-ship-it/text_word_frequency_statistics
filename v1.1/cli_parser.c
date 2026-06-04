/* cli_parser.c — 命令行参数解析实现
 * 功能：解析 main 的 argc/argv，填充 CliConfig 结构体
 * v1.1 新增模块
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cli_parser.h" /* 导入 CliConfig 结构体定义和 parse_cli、print_usage 函数声明 */


void print_usage(const char *program_name) { //定义一个函数来打印程序的使用说明，接受一个字符串参数 program_name，表示程序的名称，函数会输出如何正确使用程序以及可用的命令行选项和参数的详细信息
    printf("Usage: %s -f <file> [options]\n\n", program_name); //输出程序的使用格式，提示用户至少需要提供 -f 选项和一个文件路径参数，后续可以跟随其他可选的参数
    printf("Options:\n");
    printf("  -f <file>              Input text file (required)\n");
    printf("  -e <word1> <word2>...  Words to exclude from analysis\n");
    printf("  -t <N>                 Show top N words (default: all)\n");
    printf("  --min-length <N>       Minimum word length (default: 0)\n");
    printf("  --max-length <N>       Maximum word length (default: unlimited)\n");
    printf("  -p                     Show percentage for each word\n");
    printf("  -o <file>              Write output to file\n");
    printf("\n");
    printf("The output file includes:\n");
    printf("  - ASCII bar chart for visual word frequency comparison\n");
    printf("  - Frequency counts (and percentages if -p is specified)\n"); //输出文件的内容说明，告诉用户输出文件将包含一个 ASCII 柱状图，用于直观比较单词频率，以及每个单词的频率计数，如果使用了 -p 选项还会显示百分比信息
    printf("\nExample: %s -f sample.txt -t 10 -p -o result.txt\n", program_name); // 输出一个示例命令行，展示如何使用程序来分析 sample.txt 文件，显示前10个单词的频率和百分比，并将结果写入 result.txt 文件中
}


CliConfig parse_cli(int argc, char *argv[]) { //定义一个函数来解析命令行参数，接受两个参数：argc 表示参数的数量，argv 是一个字符串数组，包含了所有的命令行参数，函数会根据传入的参数设置配置结构体中的相应字段，并返回一个填充好的 CliConfig 结构体实例
    CliConfig cfg; //定义一个 CliConfig 结构体变量来存储解析后的配置选项，后续会根据命令行参数的内容来设置这个结构体中的字段值

    /* 默认值 */
    cfg.file_path[0] = '\0'; //将配置结构体中的 file_path 字符数组的第一个字符设置为 '\0'，表示默认情况下没有指定输入文件路径
    cfg.output_path[0] = '\0'; /* 默认不输出到文件 */
    cfg.exclude_count = 0; /* 默认没有排除词 */
    cfg.top_n = 0;           /* 0 表示所有 */
    cfg.min_length = 0;      /* 0 表示不限制 */
    cfg.max_length = 0;      /* 0 表示不限制 */
    cfg.show_percentage = 0; /* 默认不显示 */
    cfg.has_error = 0; /* 默认没有错误 */

     /* 交互模式：无参数时逐步询问 */

    /* 逐参数解析 */
    for (int i = 1; i < argc; i++) { //从 argv 数组的第二个元素开始循环遍历命令行参数，通过比较每个参数与预定义的选项来设置配置结构体中的相应字段值
        if (strcmp(argv[i], "-f") == 0) { /* -f 后面必须跟文件路径 */
            if (i + 1 < argc) { /* 确保有下一个参数作为文件路径 */
                strcpy(cfg.file_path, argv[++i]); /* 将下一个参数复制到配置结构体的 file_path 字段中，argv[++i] 表示先将 i 增加1，然后访问 argv 数组中的下一个元素作为文件路径 */
            } else {
                fprintf(stderr, "Error: -f requires a file path.\n");
                cfg.has_error = 1; /* 设置错误标志，表示在解析过程中发生了错误，例如缺少必需的 -f 选项或者没有提供文件路径参数 */
                return cfg; /* 返回配置结构体实例，可以检查 has_error 字段来判断是否解析成功 */
            }
        }
        else if (strcmp(argv[i], "-e") == 0) { //如果当前参数是 -e，表示后面跟随的是要排除的单词列表
            /* -e 后面可能跟多个词，遇到下一个 - 开头的参数停止 */
            while (i + 1 < argc && argv[i + 1][0] != '-'
                   && cfg.exclude_count < MAX_EXCLUDE_WORDS) { // 达到最大排除词数量限制
                strcpy(cfg.exclude_words[cfg.exclude_count], argv[++i]); // 将下一个参数复制到配置结构体的 exclude_words 数组中，cfg.exclude_count 作为当前排除词的索引，argv[++i] 表示先将 i 增加1，然后访问 argv 数组中的下一个元素作为排除词
                cfg.exclude_count++;
            }
        }
        else if (strcmp(argv[i], "-t") == 0) { //如果当前参数是 -t，表示后面跟随的是要显示的前 N 个单词的数量
            if (i + 1 < argc) { //确保有下一个参数作为数字
                cfg.top_n = atoi(argv[++i]);
                if (cfg.top_n < 0) cfg.top_n = 0; /* 负数不合理，改为0表示显示全部 */
            } else { /* 错误：缺少数字参数 */
                fprintf(stderr, "Error: -t requires a number.\n");
                cfg.has_error = 1;
                return cfg;
            }
        }
        else if (strcmp(argv[i], "--min-length") == 0) { //如果当前参数是 --min-length，表示后面跟随的是要设置的最小单词长度
            if (i + 1 < argc) {
                cfg.min_length = atoi(argv[++i]);
            } else {
                fprintf(stderr, "Error: --min-length requires a number.\n");
                cfg.has_error = 1;
                return cfg;
            }
        }
        else if (strcmp(argv[i], "--max-length") == 0) { //如果当前参数是 --max-length，表示后面跟随的是要设置的最大单词长度
            if (i + 1 < argc) {
                cfg.max_length = atoi(argv[++i]);
            } else {
                fprintf(stderr, "Error: --max-length requires a number.\n");
                cfg.has_error = 1;
                return cfg;
            }
        }
        else if (strcmp(argv[i], "-p") == 0) { //如果当前参数是 -p，表示用户希望在输出结果中显示每个单词的百分比信息
            cfg.show_percentage = 1; /* 设置 show_percentage 字段为1，表示在输出结果中显示百分比信息，默认情况下这个选项是关闭的，只有当用户明确指定 -p 参数时才会启用显示百分比的功能 */
        }
        else if (strcmp(argv[i], "-o") == 0) { //如果当前参数是 -o，表示后面跟随的是要输出结果的文件路径
            if (i + 1 < argc) {
                strcpy(cfg.output_path, argv[++i]);
            } else {
                fprintf(stderr, "Error: -o requires an output file path.\n");
                cfg.has_error = 1;
                return cfg;
            }
        }
        else {
            /* 未知参数，忽略 */
        }
    }

    /* -f 是必需的 */
    if (cfg.file_path[0] == '\0') { /* 如果在解析完所有参数后，配置结构体中的 file_path 字段仍然是空字符串，表示用户没有提供必需的 -f 选项或者没有提供文件路径参数，则输出错误信息并设置错误标志 */
        fprintf(stderr, "Error: -f <file> is required.\n");
        cfg.has_error = 1;
    }

    return cfg;
}
