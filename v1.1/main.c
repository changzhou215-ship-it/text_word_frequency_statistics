#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hash_table.h"
#include "word_utils.h" // 导入 clean_word 和 word_matches_length 函数声明
#include "cli_parser.h" // 导入 CliConfig 结构体定义和 parse_cli 函数声明

#define MAX_WORD_LEN 256   /* 单个单词最大读取长度 */
#define MAX_LINE_LEN 512   /* 输出行缓冲区大小 */


/* str_tolower — 字符串原地转小写*/
static void str_tolower(char *str) { //传入一个字符串指针，函数会直接修改这个字符串，将其中的所有大写字母转换为小写字母
    for (int i = 0; str[i]; i++)
        str[i] = (char)tolower((unsigned char)str[i]); //调用 tolower 函数将每个字符转换为小写。需要先将字符转换为 unsigned char，以避免负数传入导致未定义行为
}


/* is_excluded — 检查单词是否在排除列表中 */
static int is_excluded(const char *word, CliConfig *cfg) { //传入两个指针：一个指向要检查的单词字符串，另一个指向包含排除词列表和数量的配置结构体
    for (int i = 0; i < cfg->exclude_count; i++) {
        if (strcmp(word, cfg->exclude_words[i]) == 0) //使用 strcmp 函数比较单词与排除词列表中的每个词，如果找到匹配则返回 1，表示这个单词应该被排除
            return 1;
    }
    return 0;
}


/* process_file — 读取文件，统计词频
 * 返回 total_words（总单词数）
 */
static int process_file(const char *file_path, CliConfig *cfg,
                         HashTable *ht) { //传入三个参数：文件路径字符串、指向配置结构体的指针以及指向哈希表的指针
    FILE *fp = fopen(file_path, "r"); //用只读的文本模式打开指定路径的文件，返回一个指向 FILE 结构体的指针，如果打开失败则返回 NULL
    if (!fp) {
        fprintf(stderr, "Error: Could not open file '%s'\n", file_path);
        return -1;
    }

    char buffer[MAX_WORD_LEN]; //定义一个字符数组作为缓冲区，用于存储从文件中读取的单词
    int total_words = 0; //定义一个整数变量来统计总单词数，初始值为0

    while (fscanf(fp, "%255s", buffer) == 1) { //使用 fscanf 函数从文件中读取单词，%255s 表示最多读取255个字符，最后一个字符留给 '\0' 结束符，buffer 数组用来存储读取的单词
        total_words++;

        /* Step 1: 转小写 */
        str_tolower(buffer); //调用 str_tolower 函数将读取的单词转换为小写，确保统计时大小写不敏感

        /* Step 2: 标点清洗 */
        char *cleaned = clean_word(buffer); //调用 clean_word 函数对转换为小写的单词进行清洗，去除首尾的标点符号，返回一个指向清洗后单词的指针，如果清洗后单词为空返回 NULL
        if (!cleaned) continue;    /* 全标点，跳过 */

        /* Step 3: 排除词检查 */
        if (is_excluded(cleaned, cfg)) continue; //调用 is_excluded 函数检查清洗后的单词是否在排除词列表中

        /* Step 4: 长度过滤 */
        if (!word_matches_length(cleaned, cfg->min_length, cfg->max_length)) //调用 word_matches_length 函数检查清洗后的单词是否符合最小和最大长度的要求
            continue;

        /* Step 5: 插入哈希表 */
        ht_insert(ht, cleaned); //调用 ht_insert 函数将符合条件的单词插入哈希表中进行统计，如果单词已经存在则更新其频率计数
    }

    fclose(fp); //关闭文件流，释放系统资源
    return total_words; //返回统计的总单词数，供后续处理使用
}


/* get_max_count — 获取最高频率（用于柱状图缩放）*/
static int get_max_count(WordPair *sorted, int count) { //传入一个指向 WordPair 结构体数组的指针和数组中元素的数量，函数返回数组中第一个元素的 count 值，即最高频率，因为数组已经按频率降序排列
    if (count <= 0) return 1; // 如果没有单词，返回1以避免除以零的情况
    return sorted[0].count;  /* 已按频率降序排列，首元素最高 */
}


/* output_results — 输出结果到屏幕和可选文件 */
static void output_results(WordPair *sorted, int unique_count,
                            int total_words, CliConfig *cfg) { //传入一个指向 WordPair 结构体数组的指针、数组中不重复单词的数量、总单词数以及指向配置结构体的指针，函数负责将统计结果格式化输出到屏幕和可选的输出文件中
    FILE *out_fp = NULL; //定义一个文件指针变量，用于指向输出文件，如果用户指定了输出文件路径则打开该文件进行写入，否则保持为 NULL 以表示只输出到屏幕

    if (cfg->output_path[0] != '\0') { //如果配置结构体中的 output_path 字符串不为空，表示用户指定了输出文件路径，则尝试打开该文件进行写入
        out_fp = fopen(cfg->output_path, "w"); //以写入模式打开指定路径的文件，如果文件不存在则创建，如果文件已存在则覆盖，返回一个指向 FILE 结构体的指针，如果打开失败则返回 NULL
        if (!out_fp) { //如果打开文件失败，输出警告信息并继续只输出到屏幕
            fprintf(stderr, "Warning: Could not open output file '%s', "
                    "outputting to screen only.\n", cfg->output_path); //输出警告信息到标准错误流，告知用户无法打开指定的输出文件，并将继续只输出到屏幕
        }
    }

    int display_count = (cfg->top_n > 0 && cfg->top_n < unique_count) //使用三目运算符确定实际要显示的单词数量，如果用户指定了一个正数的 top_n 并且这个值小于不重复单词的总数，则显示 top_n 个单词，否则显示所有不重复的单词
                        ? cfg->top_n : unique_count;
    int max_count = get_max_count(sorted, display_count); //调用 get_max_count 函数获取要显示的单词中最高的频率计数值，用于后续绘制柱状图时进行缩放，以确保最长的柱子占满预设的最大宽度

    char line[MAX_LINE_LEN]; //定义一个字符数组作为输出行的缓冲区，用于存储每行输出的格式化字符串，包含单词、柱状图和计数信息，最大长度由 MAX_LINE_LEN 定义，以确保足够容纳最长的输出行

    /* 分隔线和统计摘要 */
    fprintf(stdout, "\n"); //输出一个空行作为分隔符，增强输出的可读性,stdout 是标准输出流，通常指向屏幕
    if (out_fp) fprintf(out_fp, "\n"); //如果输出文件已成功打开，则也输出一个空行到文件中，保持屏幕和文件输出的一致性

    fprintf(stdout, "=== Word Frequency Statistics ===\n");
    if (out_fp) fprintf(out_fp, "=== Word Frequency Statistics ===\n");

    fprintf(stdout, "Total words   : %d\n", total_words);
    if (out_fp) fprintf(out_fp, "Total words   : %d\n", total_words);

    fprintf(stdout, "Unique words  : %d\n", unique_count);
    if (out_fp) fprintf(out_fp, "Unique words  : %d\n", unique_count);

    fprintf(stdout, "Showing Top   : %d\n", display_count);
    if (out_fp) fprintf(out_fp, "Showing Top   : %d\n", display_count);

    fprintf(stdout, "\n");
    if (out_fp) fprintf(out_fp, "\n");

    fprintf(stdout, "%-20s %-50s %s\n", "Word", "Bar", "Count"); /*输出表头，使用格式化字符串指定列宽，第一列为单词（20字符宽），第二列为柱状图（50字符宽），第三列为计数（不指定宽度，自动调整）
                                                                   %-20s 表示左对齐的字符串占20个字符宽，%-50s 表示左对齐的字符串占50个字符宽，%s 表示一个字符串*/
    if (out_fp) fprintf(out_fp, "%-20s %-50s %s\n", "Word", "Bar", "Count");

    /* 分隔线 */
    for (int i = 0; i < 90; i++) { fputc('-', stdout); if (out_fp) fputc('-', out_fp); } //输出90个连字符 '-' 作为分隔线，增强输出的可读性，fputc 函数用于向指定的输出流写入一个字符，这里同时写入到屏幕和文件（如果文件已打开）
    fprintf(stdout, "\n");
    if (out_fp) fprintf(out_fp, "\n"); //输出一个换行符，结束分隔线的输出，准备开始输出单词统计结果 

    /* 逐词输出柱状图 */
    for (int i = 0; i < display_count; i++) { //循环遍历要显示的单词数量，输出每个单词的统计信息和对应的柱状图
        draw_bar(sorted[i].word, sorted[i].count, max_count,
                 total_words, cfg->show_percentage, line, sizeof(line)); //调用 draw_bar 函数生成每个单词的输出行，传入单词字符串、单词计数、最高频率计数、总单词数、是否显示百分比的标志以及用于存储输出行的缓冲区和缓冲区大小，draw_bar 函数会将格式化后的输出行写入 line 缓冲区中
        fprintf(stdout, "%s\n", line); // 将生成的输出行打印到屏幕上，line 是一个字符串，包含了单词、柱状图和计数信息
        if (out_fp) fprintf(out_fp, "%s\n", line); // 保持屏幕和文件输出的一致性
    }

    /* --- 结尾 --- */
    for (int i = 0; i < 90; i++) { fputc('-', stdout); if (out_fp) fputc('-', out_fp); } //输出90个连字符 '-' 作为结尾的分隔线
    fprintf(stdout, "\n");
    if (out_fp) fprintf(out_fp, "\n");

    if (out_fp) { //如果输出文件已成功打开，关闭文件流并提示用户输出文件的路径
        fclose(out_fp);
        printf("\n[Output saved to: %s]\n", cfg->output_path); //输出提示信息到屏幕，告知用户统计结果已保存到指定的输出文件路径
    }
}


/* main — 主入口
 * 两种运行方式：
 *   1. 命令行：main.exe -f <file> [options]
 *   2. 交互式：直接运行 main.exe，逐步引导输入
 * 交互流程：输入文件 → 排除词 → 输出文件 → 处理 → 显示总数 → 询问 Top N（告知上限）→ 询问百分比 → 排序输出
 */
int main(int argc, char *argv[]) { //主函数，传入命令行参数的数量和参数值的字符串数组，函数根据传入的参数决定是进入命令行模式还是交互式模式
    CliConfig cfg; //定义一个 CliConfig 结构体变量，用于存储命令行参数或交互式输入的配置选项，包括文件路径、排除词列表、显示选项等
    char interactive_path[512]; //定义一个字符数组作为交互式输入的文件路径缓冲区，最大长度为512字符，用于存储用户在交互式模式下输入的文件路径

    /* 初始化默认值 */
    memset(&cfg, 0, sizeof(cfg));
    cfg.show_percentage = 1;  /* 交互模式默认显示百分比 */
    cfg.min_length = 0; // 交互模式默认不限制最小/最大长度
    cfg.max_length = 0;

    /* 判断运行模式 */
    if (argc == 1) { //如果命令行参数数量为1，表示用户没有提供任何参数，程序将进入交互式模式，通过逐步引导用户输入所需的信息来配置程序的运行选项
        /* 交互式模式 — 3 步输入，然后处理 */

        /* 1. 输入文件 */
        printf("Enter input file path: ");
        if (!fgets(interactive_path, sizeof(interactive_path), stdin)) { //使用 fgets 函数从标准输入流读取用户输入的文件路径，存储在 interactive_path 缓冲区中，如果读取失败则输出错误信息并退出程序
            fprintf(stderr, "Error: No input.\n");
            return 1;
        }
        interactive_path[strcspn(interactive_path, "\r\n")] = '\0'; //去除输入末尾的换行符，strcspn 函数返回字符串中第一个出现 '\r' 或 '\n' 的位置，将该位置的字符替换为 '\0' 以结束字符串
        if (interactive_path[0] == '\0') { //如果用户输入的文件路径为空字符串，则输出错误信息并退出程序
            fprintf(stderr, "Error: Empty file path.\n");
            return 1;
        }

        /* 2. 排除词（先问，因为影响后续处理） */
        printf("Words to exclude (space-separated, Enter for none): "); //提示用户输入要排除的单词列表，单词之间用空格分隔，如果按 Enter 则表示不排除任何单词
        {
            char buf[512]; //定义一个字符数组作为输入缓冲区，用于存储用户输入的排除词列表，最大长度为512字符
            if (fgets(buf, sizeof(buf), stdin)) { //使用 fgets 函数从标准输入流读取用户输入的排除词列表，存储在 buf 缓冲区中，如果读取成功则继续处理输入
                buf[strcspn(buf, "\r\n")] = '\0'; //去除输入末尾的换行符，strcspn 函数返回字符串中第一个出现 '\r' 或 '\n' 的位置，将该位置的字符替换为 '\0' 以结束字符串
                if (buf[0] != '\0') { //如果用户输入的排除词列表不为空字符串，则继续处理输入，将排除词分割并存储到配置结构体中
                    /* 手动切分：逐字符扫描，遇空格写入新词 */
                    char word[64]; //定义一个字符数组作为单个排除词的缓冲区，最大长度为64字符，用于存储从输入中分割出的每个排除词
                    int wi = 0; //定义一个整数变量作为当前排除词的字符索引，初始值为0，用于在 word 缓冲区中构建当前排除词字符串
                    cfg.exclude_count = 0; //初始化配置结构体中的排除词数量为0，后续在分割输入时会根据实际分割出的排除词数量进行更新
                    for (int i = 0; buf[i] && cfg.exclude_count < MAX_EXCLUDE_WORDS; i++) { //循环遍历输入缓冲区中的每个字符，直到遇到字符串结束符 '\0' 或者达到最大排除词数量的限制，在循环中根据空格分割输入并构建排除词字符串
                        if (buf[i] == ' ') { //如果当前字符是空格，表示一个排除词的结束，同时更新排除词数量
                            if (wi > 0) { //如果当前构建的排除词字符串不为空，则将其转换为小写并存储到配置结构体的排除词列表中
                                word[wi] = '\0';
                                str_tolower(word);
                                strcpy(cfg.exclude_words[cfg.exclude_count], word); /* 将构建的排除词字符串复制到配置结构体的排除词列表中，cfg.exclude_count 作为当前排除词的索引 */
                                cfg.exclude_count++;
                                wi = 0; //重置排除词字符索引，准备构建下一个排除词字符串
                            }
                        } else {
                            if (wi < 63) word[wi++] = buf[i]; /* 如果当前字符不是空格，则将其添加到当前排除词字符串的缓冲区中，并更新排除词字符索引，确保不超过缓冲区大小限制 */
                        }
                    }
                    /* 最后一个词（行末没有空格） */
                    if (wi > 0 && cfg.exclude_count < MAX_EXCLUDE_WORDS) { //如果在循环结束后还有一个未处理的排除词字符串（行末没有空格），则将其转换为小写并存储到配置结构体的排除词列表中
                        word[wi] = '\0'; //结束当前排除词字符串
                        str_tolower(word);
                        strcpy(cfg.exclude_words[cfg.exclude_count], word); //将最后一个排除词字符串复制到配置结构体的排除词列表中，cfg.exclude_count 作为当前排除词的索引
                        cfg.exclude_count++;
                    }
                }
            }
        }

        /* 3. 输出文件（默认 wordfreq_result.txt） */
        printf("Enter output file name (default: wordfreq_result.txt): ");
        if (!fgets(cfg.output_path, sizeof(cfg.output_path), stdin)) //使用 fgets 函数从标准输入流读取用户输入的输出文件路径，存储在配置结构体的 output_path 字符数组中，如果读取失败则继续使用默认值
            strcpy(cfg.output_path, "wordfreq_result.txt");
        cfg.output_path[strcspn(cfg.output_path, "\r\n")] = '\0'; //去除输入末尾的换行符，strcspn 函数返回字符串中第一个出现 '\r' 或 '\n' 的位置，将该位置的字符替换为 '\0' 以结束字符串
        if (cfg.output_path[0] == '\0') //如果用户输入的输出文件路径为空字符串，则使用默认值 "wordfreq_result.txt" 作为输出文件路径
            strcpy(cfg.output_path, "wordfreq_result.txt");

        /* 用 parse_cli 验证文件路径 */ 
        {
            char *fake_argv[] = { argv[0], "-f", interactive_path }; //构造一个假的命令行参数数组，包含程序名、-f 选项和用户输入的文件路径
            CliConfig parsed = parse_cli(3, fake_argv); //调用 parse_cli 函数来验证文件路径的有效性,'3' 表示 fake_argv 数组中有三个元素
            strcpy(cfg.file_path, parsed.file_path); //并将解析结果中的 file_path 和 has_error 字段复制到配置结构体中，以确保后续处理使用正确的文件路径并能够检测到任何解析错误
            cfg.has_error = parsed.has_error;
        }
    } else {
        /* 命令行模式：正常解析 */
        cfg = parse_cli(argc, argv); //调用 parse_cli 函数来解析命令行参数，传入 argc 和 argv，函数会根据传入的参数设置配置结构体中的相应字段，包括文件路径、排除词列表、显示选项等，并返回一个填充好的 CliConfig 结构体实例
    }

    if (cfg.has_error) { //如果在解析命令行参数或交互式输入过程中检测到任何错误（例如缺少必需的参数、无效的选项值等），则输出使用说明并退出程序
        print_usage(argv[0]);
        return 1;
    }

    /* 处理文件：统计词频 */
    HashTable *ht = ht_create(); //调用 ht_create 函数创建一个新的哈希表实例，用于存储单词及其频率计数，函数返回一个指向 HashTable 结构体的指针，如果创建失败则返回 NULL
    int total_words = process_file(cfg.file_path, &cfg, ht); //调用 process_file 函数来读取指定路径的文件并统计单词频率，传入文件路径、配置结构体指针和哈希表指针，函数返回统计的总单词数，如果处理过程中发生错误（例如无法打开文件）则返回 -1

    if (total_words < 0) { //如果 process_file 函数返回的总单词数小于0，表示在处理文件时发生了错误（例如无法打开文件），则输出错误信息并退出程序
        ht_destroy(ht); //调用 ht_destroy 函数来销毁哈希表实例，释放相关资源，确保在程序退出前进行适当的清理
        return 1;
    }

    if (total_words == 0) { //如果 process_file 函数返回的总单词数为0，表示在文件中没有找到任何有效的单词（可能是文件为空或者所有内容都被清洗掉了），则输出提示信息并退出程序
        printf("No valid words found in file.\n");
        ht_destroy(ht);
        return 0;
    }

    /* 交互模式：拿到实际数量后再问 Top N */
    int unique_count; //定义一个整数变量来接收哈希表中不重复单词的总数，后续会通过调用 ht_to_array 函数来获取这个值
    WordPair *sorted = ht_to_array(ht, &unique_count); //调用 ht_to_array 函数将哈希表中的单词及其频率计数转换为一个 WordPair 结构体数组，传入哈希表指针和一个指向整数的指针来接收不重复单词的总数，函数返回一个指向 WordPair 数组的指针，如果转换失败则返回 NULL
    qsort(sorted, unique_count, sizeof(WordPair), cmp_by_freq); //调用 qsort 函数对 WordPair 数组进行排序，按照频率计数进行降序排序，传入数组指针、元素数量、每个元素的大小以及比较函数 cmp_by_freq 的指针

    if (argc == 1) { //如果没有提供命令行参数，表示程序处于交互式模式
        /* 4. Top N — 告知实际上限 */
        char top_buf[16]; // 定义一个字符数组作为输入缓冲区，用于存储用户输入的要显示的前 N 个单词的数量，最大长度为16字符
        printf("Show top N words (0 or Enter = all, max %d): ", unique_count);
        if (fgets(top_buf, sizeof(top_buf), stdin) //从标准输入流读取用户输入的要显示的前 N 个单词的数量，存储在 top_buf 缓冲区中，sizeof(top_buf) 确保不会发生缓冲区溢出
            && top_buf[0] != '\r' && top_buf[0] != '\n') { //如果读取成功并且用户输入的不是空行（即 Enter）
            cfg.top_n = atoi(top_buf); //调用C标准库的 atoi 函数将用户输入的字符串转换为整数，存储在配置结构体的 top_n 字段中，atoi 函数会解析字符串中的数字并返回对应的整数值
            if (cfg.top_n < 0 || cfg.top_n > unique_count) cfg.top_n = 0; // 如果用户输入的数字小于0或者大于不重复单词的总数，则将 top_n 设置为0，表示显示所有单词
        }

        /* 5. 百分比（默认 y） */
        char pct_buf[8]; //定义一个字符数组作为输入缓冲区，用于存储输入的是否显示百分比的选项，最大长度为8字符
        printf("Show percentage? (Y/n, default Y): ");
        if (fgets(pct_buf, sizeof(pct_buf), stdin))
            cfg.show_percentage = (pct_buf[0] != 'n' && pct_buf[0] != 'N'); //从标准输入流读取用户输入的是否显示百分比的选项，存储在 pct_buf 缓冲区中，
                                                                            //如果读取成功则检查用户输入的第一个字符，如果是 'n' 或 'N' 则将 show_percentage 设置为0，否则设置为1，默认情况下交互式模式会显示百分比
        else
            cfg.show_percentage = 1; //如果读取失败，则默认显示百分比

        printf("\n");  /* 空行分隔 */
    }

    /* 输出结果 */
    output_results(sorted, unique_count, total_words, &cfg); //调用 output_results 函数来输出统计结果，传入排序后的 WordPair 数组、不重复单词的总数、总单词数以及配置结构体指针，函数会根据配置选项将结果格式化输出到屏幕和可选的输出文件中

    /* 清理 */
    free(sorted); //释放之前通过 ht_to_array 函数分配的 WordPair 数组的内存，确保在程序退出前进行适当的清理
    ht_destroy(ht); //调用 ht_destroy 函数来销毁哈希表实例，释放相关资源

    /* 防止窗口闪退（验收后可删除） */
    fflush(stdin);
    printf("\n=== Press Enter to exit... ===");
    getchar();
    return 0;
}
