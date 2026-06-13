/* word_utils.c — 单词处理工具实现
 * 功能：单词清洗、长度过滤、百分比计算、ASCII 柱状图生成
 * v1.1 新增模块
 * [v2.0 扩展] 新增停用词、TTR、字符统计、alpha排序、HTML输出
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
    if (c >= 0x80) return 1;  /* [v2.0] 非ASCII字符当标点（弯引号、长破折号等） */
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
     /* 定义一个整数变量 bar_len 来存储计算得到的柱长，首先检查 max_count 是否大于 0，如果是则根据 count 与 max_count 的比例计算柱长，并乘以预设的最大宽度 BAR_MAX_WIDTH，如果计算得到的柱长小于 1 且 count 大于 0，则将柱长设置为 1，确保至少显示一格柱子 */
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

    /* 柱状图部分 — 固定 BAR_MAX_WIDTH 列宽，确保 Count 列对齐 */
    char bar_str[BAR_MAX_WIDTH + 1];
    int j;
    for (j = 0; j < bar_len; j++) bar_str[j] = '#';
    for (; j < BAR_MAX_WIDTH; j++) bar_str[j] = ' ';
    bar_str[BAR_MAX_WIDTH] = '\0';
    pos += snprintf(buf + pos, buf_size - pos, "%s", bar_str);

    /* 数字部分 */
    if (show_pct) /* 如果选择显示百分比 */
        pos += snprintf(buf + pos, buf_size - pos, " %d  (%.1f%%)", count, pct); /* 使用 snprintf 函数将单词的出现次数和百分比格式化写入输出缓冲区，格式化字符串 " %d  (%.1f%%)" 表示先输出一个整数（count），然后输出一个浮点数（pct）保留一位小数，并在数字之间添加适当的空格和括号，snprintf 会返回实际写入的字符数，将其累加到 pos 变量中，以更新当前写入位置 */
    else
        pos += snprintf(buf + pos, buf_size - pos, " %d", count); /* 如果不显示百分比，则只输出单词的出现次数，格式化字符串 " %d" 表示先输出一个整数（count），前面添加一个空格以分隔单词和数字，snprintf 同样会更新当前写入位置 */

    /* 不设终止符（snprintf 已设置） */
}


/* ==============================================================
 * 以下为 [v2.0 新增] 函数
 * ============================================================== */

/* === [v2.0] 停用词列表 (NULL结尾，免数数量) === */
static const char *stopwords[] = {
    "a","about","above","after","again","against","all","am","an","and",
    "any","are","aren't","as","at","be","because","been","before","being",
    "below","between","both","but","by","can't","cannot","could","couldn't",
    "did","didn't","do","does","doesn't","doing","don't","down","during","each",
    "few","for","from","further","had","hadn't","has","hasn't","have","haven't",
    "having","he","he'd","he'll","he's","her","here","here's","hers","herself",
    "him","himself","his","how","how's","i","i'd","i'll","i'm","i've",
    "if","in","into","is","isn't","it","it's","its","itself","let's",
    "me","more","most","mustn't","my","myself","no","nor","not","of",
    "off","on","once","only","or","other","ought","our","ours","ourselves",
    "out","over","own","same","shan't","she","she'd","she'll","she's",
    "should","shouldn't","so","some","such","than","that","that's","the",
    "their","theirs","them","themselves","then","there","there's","these",
    "they","they'd","they'll","they're","they've","this","those","through",
    "to","too","under","until","up","very","was","wasn't","we","we'd",
    "we'll","we're","we've","were","weren't","what","what's","when",
    "when's","where","where's","which","while","who","who's","whom",
    "why","why's","with","won't","would","wouldn't","you","you'd",
    "you'll","you're","you've","your","yours","yourself","yourselves",
    NULL  /* [v2.0] 哨兵，标志数组结束 */
};

/* [v2.0] is_numeric_word — 检查单词是否为数字（纯数字或类似 "12-24"） */
int is_numeric_word(const char *word) {
    if (!word || !*word) return 0;
    /* 首字符为数字即视为数字词 */
    return (word[0] >= '0' && word[0] <= '9');
}

/* [v2.0] is_stopword — 检查单词是否为停用词 */
int is_stopword(const char *word) {
    for (int i = 0; stopwords[i] != NULL; i++)
        if (strcmp(word, stopwords[i]) == 0)
            return 1;
    return 0;
}

/* [v2.0] calc_ttr — 计算词汇多样性 */
double calc_ttr(int unique, int total) {
    if (total <= 0) return 0.0;
    return ((double)unique / total) * 100.0;
}

/* [v2.0] count_chars — 统计文件字符数 */
void count_chars(const char *file_path, int *chars_with_spaces,
                 int *chars_without_spaces) {
    *chars_with_spaces = 0;
    *chars_without_spaces = 0;
    FILE *fp = fopen(file_path, "r");
    if (!fp) return;
    int c;
    while ((c = fgetc(fp)) != EOF) {
        (*chars_with_spaces)++;
        if (c != ' ' && c != '\t' && c != '\n' && c != '\r')
            (*chars_without_spaces)++;
    }
    fclose(fp);
}

/* [v2.0] cmp_by_alpha — qsort 比较函数（字母序）
 * 先按首字母 A→Z，同字母按频率降序 */
int cmp_by_alpha(const void *a, const void *b) {
    const WordPair *pa = (const WordPair *)a;
    const WordPair *pb = (const WordPair *)b;
    char ca = pa->word[0];
    char cb = pb->word[0];
    if (ca != cb) return ca - cb;          /* 首字母 A→Z */
    /* 同字母按频率降序，频率相同则按字母序兜底 */
    if (pb->count != pa->count)
        return pb->count - pa->count;
    return strcmp(pa->word, pb->word);
}

/* [v2.0] output_html — 生成 HTML 格式报告 */
void output_html(WordPair *sorted, int unique_count, int total_words,
                 int max_count, int show_pct,
                 int chars_with_spaces, int chars_without_spaces,
                 double ttr, const char *output_path) {
    FILE *fp = fopen(output_path, "w");
    if (!fp) return;

    fprintf(fp, "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n");
    fprintf(fp, "<meta charset=\"UTF-8\">\n");
    fprintf(fp, "<title>Word Frequency Report</title>\n");
    fprintf(fp, "<style>\n");
    fprintf(fp, "body{font-family:Arial,sans-serif;margin:40px;background:#0f0f14;color:#e4e4ec}\n");
    fprintf(fp, "h1{color:#7c5cfc;border-bottom:2px solid #2a2a3a;padding-bottom:10px}\n");
    fprintf(fp, ".summary{background:#1a1a24;padding:16px 24px;border-radius:8px;margin-bottom:24px}\n");
    fprintf(fp, "table{width:100%%;border-collapse:collapse}\n");
    fprintf(fp, "th{background:#7c5cfc;color:#fff;padding:10px;text-align:left}\n");
    fprintf(fp, "td{padding:8px 10px;border-bottom:1px solid #2a2a3a}\n");
    fprintf(fp, "tr:hover{background:#1a1a24}\n");
    fprintf(fp, ".bar{background:#7c5cfc;height:16px;border-radius:3px;min-width:2px}\n");
    fprintf(fp, "</style>\n</head>\n<body>\n");

    fprintf(fp, "<h1>=== Word Frequency Report ===</h1>\n");
    fprintf(fp, "<div class=\"summary\">\n");
    fprintf(fp, "<p>Total words: %d &nbsp;|&nbsp; Unique words: %d</p>\n", total_words, unique_count);
    fprintf(fp, "<p>Chars (with spaces): %d &nbsp;|&nbsp; Chars (no spaces): %d</p>\n", chars_with_spaces, chars_without_spaces);
    fprintf(fp, "<p>TTR (lexical diversity): %.1f%%</p>\n", ttr);
    fprintf(fp, "</div>\n");

    fprintf(fp, "<table>\n<tr><th>Word</th><th>Bar</th><th>Count</th>");
    if (show_pct) fprintf(fp, "<th>%%</th>");
    fprintf(fp, "</tr>\n");

    int bar_max_px = 300;
    for (int i = 0; i < unique_count; i++) {
        double pct = calc_percentage(sorted[i].count, total_words);
        int bar_w = 0;
        if (max_count > 0)
            bar_w = (int)(((double)sorted[i].count / max_count) * bar_max_px);
        if (bar_w < 2 && sorted[i].count > 0) bar_w = 2;

        fprintf(fp, "<tr><td>%s</td><td><div class=\"bar\" style=\"width:%dpx\"></div></td><td>%d</td>",
                sorted[i].word, bar_w, sorted[i].count);
        if (show_pct) fprintf(fp, "<td>%.1f%%</td>", pct);
        fprintf(fp, "</tr>\n");
    }

    fprintf(fp, "</table>\n</body>\n</html>\n");
    fclose(fp);
}
