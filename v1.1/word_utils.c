/* word_utils.c — 单词处理工具实现
 * 功能：单词清洗、长度过滤、百分比计算、ASCII 柱状图生成
 * v1.1 新增模块
*/

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "word_utils.h" /* 导入 clean_word、word_matches_length、calc_percentage、draw_bar 函数声明 */

/* is_punct — 判断字符是否为标点（内部辅助函数）
 * 参数 c：待检查的字符
 * 返回：1 是标点，0 不是
 * 标点范围覆盖常见中英文标点，不含连字符 - 和撇号 '
*/
static int is_punct_char(int c) {
    return (c == '.' || c == ',' || c == '!' || c == '?' ||
            c == ';' || c == ':' || c == '\"' || c == '(' ||
            c == ')' || c == '[' || c == ']' || c == '{' ||
            c == '}' || c == '/' || c == '\\' || c == '@' ||
            c == '#' || c == '$' || c == '%' || c == '^' ||
            c == '&' || c == '*' || c == '|' || c == '~' ||
            c == '`' || c == '_' || c == '=' || c == '+' ||
            c == '<' || c == '>');
}


char *clean_word(char *word) { // 定义一个函数来清洗单词，接受一个字符串指针作为参数，函数会去除这个字符串开头和结尾的标点符号，并返回一个指向清洗后单词的指针
    if (!word || !*word) return NULL; /* 如果传入的字符串指针为 NULL 或者指向的字符串为空，则返回 NULL，表示没有有效的单词可以清洗 */

    char *start = word; /* 定义一个指针 start，初始值指向传入的字符串的开头，用于遍历字符串并跳过开头的标点符号 */
    char *end   = word + strlen(word) - 1; /* 定义一个指针 end，初始值指向传入的字符串的结尾，用于遍历字符串并跳过结尾的标点符号，strlen(word) - 1 表示最后一个字符的位置 */

    /* 跳过开头标点 */
    while (*start && is_punct_char((unsigned char)*start)) //使用 is_punct_char 函数检查 start 指针当前指向的字符是否为标点，如果是则将 start 指针向后移动一位，继续检查下一个字符，直到遇到非标点字符或者字符串结束
        start++;

    /* 跳过结尾标点 */
    while (end > start && is_punct_char((unsigned char)*end)) //使用 is_punct_char 函数检查 end 指针当前指向的字符是否为标点，如果是则将 end 指针向前移动一位，继续检查上一个字符，直到遇到非标点字符或者 end 指针不再大于 start 指针（即已经检查完所有字符）
        end--;

    /* 如果全是标点或被标点包成空，返回 NULL */
    if (start > end) return NULL;

    /* 原地截断：把结束位置后的字符设为 '\0' */
    *(end + 1) = '\0';

    return start;
}


int word_matches_length(const char *word, int min_len, int max_len) { //定义一个函数来检查单词的长度是否在指定的最小和最大范围内，接受一个字符串指针和两个整数参数，分别表示最小长度和最大长度
    if (!word) return 0; /* 如果传入的字符串指针为 NULL，则返回 0，表示不匹配 */

    int len = strlen(word); /* 定义一个整数变量 len 来存储传入字符串的长度，使用 strlen 函数计算字符串的长度 */

     /* 检查长度范围 */
     /* 如果 min_len 大于 0 且 len 小于 min_len，返回 0，表示单词长度不符合最小长度要求 */
     /* 如果 max_len 大于 0 且 len 大于 max_len，返回 0，表示单词长度超过最大长度要求 */
     /* 否则返回 1，表示单词长度符合要求 */
    if (min_len > 0 && len < min_len) return 0;
    if (max_len > 0 && len > max_len) return 0;
    return 1;
}


double calc_percentage(int count, int total) { //定义一个函数来计算百分比，接受两个整数参数，分别表示单词出现的次数和总单词数，函数会返回一个表示百分比的浮点数
     /* 如果 total 小于或等于 0，则返回 0.0，表示无法计算百分比 */
     /* 否则计算 count 与 total 的比例，并乘以 100 返回百分比值 */
    if (total <= 0) return 0.0;
    return ((double)count / total) * 100.0;
}


void draw_bar(const char *word, int count, int max_count, int total,
              int show_pct, char *buf, int buf_size) { //定义一个函数来绘制水平 ASCII 柱状图行，接受多个参数，包括单词字符串、出现次数、最高频率、总单词数、是否显示百分比的标志以及输出缓冲区和缓冲区大小，函数会根据这些参数生成一条格式化的柱状图行，并将结果写入提供的缓冲区中
    if (!word || !buf || buf_size <= 0) return; /* 如果传入的单词字符串指针、输出缓冲区指针为 NULL，或者缓冲区大小不大于 0，则直接返回，不进行任何操作 */

     /* 计算柱长：count / max_count * BAR_MAX_WIDTH */
     /* 定义一个整数变量 bar_len 来存储计算得到的柱状图长度，首先检查 max_count 是否大于 0，如果是则根据 count 与 max_count 的比例计算柱长，并乘以预设的最大宽度 BAR_MAX_WIDTH，如果计算得到的柱长小于 1 且 count 大于 0，则将柱长设置为 1，确保至少显示一格柱子 */
     /* 如果 max_count 不大于 0，则柱长保持为 0，表示没有数据可供绘制柱状图 */
    int bar_len = 0;
    if (max_count > 0) {
        bar_len = (int)(((double)count / max_count) * BAR_MAX_WIDTH);
        if (bar_len < 1 && count > 0) bar_len = 1;  /* 至少显示一格 */
    }

    /* 百分比 */
    double pct = calc_percentage(count, total); /* 调用 calc_percentage 函数计算当前单词出现次数占总单词数的百分比，结果存储在 pct 变量中，供后续格式化输出使用 */

    /* 组装输出行 */
    int pos = 0; /* 定义一个整数变量 pos 来跟踪当前在输出缓冲区中的写入位置，初始值为 0，表示从缓冲区的开头开始写入 */

    /* 单词左对齐 20 字符宽 */
    pos += snprintf(buf + pos, buf_size - pos, "%-20s ", word); /* 使用 snprintf 函数将单词字符串格式化写入输出缓冲区，格式化字符串 "%-20s " 表示左对齐的字符串占 20 个字符宽，并在单词后添加一个空格，snprintf 会返回实际写入的字符数，将其累加到 pos 变量中，以更新当前写入位置 */

    /* 柱状图部分 — 用 # 字符，纯 ASCII 兼容所有终端编码 */
    for (int i = 0; i < bar_len && pos < buf_size - 1; i++) //使用一个循环来生成柱状图部分，根据计算得到的柱长 bar_len 来决定输出多少个 # 字符，循环条件确保不会超过缓冲区的剩余空间，避免缓冲区溢出
        pos += snprintf(buf + pos, buf_size - pos, "#"); /* 在循环中使用 snprintf 函数将一个 # 字符写入输出缓冲区，每次写入后更新 pos 变量以跟踪当前写入位置，确保不会超过缓冲区大小 */

    /* 数字部分 */
    if (show_pct) /* 如果选择显示百分比 */
        pos += snprintf(buf + pos, buf_size - pos, " %d  (%.1f%%)", count, pct); /* 使用 snprintf 函数将单词的出现次数和百分比格式化写入输出缓冲区，格式化字符串 " %d  (%.1f%%)" 表示先输出一个整数（count），然后输出一个浮点数（pct）保留一位小数，并在数字之间添加适当的空格和括号，snprintf 会返回实际写入的字符数，将其累加到 pos 变量中，以更新当前写入位置 */
    else
        pos += snprintf(buf + pos, buf_size - pos, " %d", count); /* 如果不显示百分比，则只输出单词的出现次数，格式化字符串 " %d" 表示先输出一个整数（count），前面添加一个空格以分隔单词和数字，snprintf 同样会更新当前写入位置 */

    /* 不设终止符（snprintf 已设置） */
}
