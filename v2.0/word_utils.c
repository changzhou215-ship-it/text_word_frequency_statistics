/* word_utils.c — 单词处理工具实现
 * 功能：单词清洗、长度过滤、百分比计算、ASCII 柱状图生成
 * v1.1 新增模块
 * v2.0 扩展 新增停用词、TTR、字符统计、alpha排序、HTML输出
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

/* 检查单词是否包含非 ASCII 字节（>= 0x80），含 em dash、弯引号等即丢弃 */
int has_non_ascii(const char *word) {
    if (!word) return 0;
    for (const char *p = word; *p; p++)
        if ((unsigned char)*p >= 0x80) return 1;
    return 0;
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
    for (j = 0; j < bar_len; j++) bar_str[j] = '#'; /* 根据计算得到的柱长 bar_len，在 bar_str 数组中填充 '#' 字符，表示柱状图的可视部分，直到达到 bar_len 的长度 */
    for (; j < BAR_MAX_WIDTH; j++) bar_str[j] = ' '; /* 在 bar_str 数组中剩余的位置填充空格字符，确保柱状图部分占满预设的最大宽度 BAR_MAX_WIDTH，这样可以保持输出的整齐对齐 */
    bar_str[BAR_MAX_WIDTH] = '\0'; /* 在 bar_str 数组的末尾添加一个空字符 '\0'，以确保它是一个合法的 C 字符串，可以安全地使用字符串函数进行处理 */
    pos += snprintf(buf + pos, buf_size - pos, "%s", bar_str); /* 将生成的柱状图字符串 bar_str 格式化写入输出缓冲区，snprintf 会返回实际写入的字符数，将其累加到 pos 变量中，以更新当前写入位置 */

    /* 数字部分 */
    if (show_pct) /* 如果选择显示百分比 */
        pos += snprintf(buf + pos, buf_size - pos, " %d  (%.1f%%)", count, pct); /* 使用 snprintf 函数将单词的出现次数和百分比格式化写入输出缓冲区，格式化字符串 " %d  (%.1f%%)" 表示先输出一个整数（count），然后输出一个浮点数（pct）保留一位小数，并在数字之间添加适当的空格和括号，snprintf 会返回实际写入的字符数，将其累加到 pos 变量中，以更新当前写入位置 */
    else
        pos += snprintf(buf + pos, buf_size - pos, " %d", count); /* 如果不显示百分比，则只输出单词的出现次数，格式化字符串 " %d" 表示先输出一个整数（count），前面添加一个空格以分隔单词和数字，snprintf 同样会更新当前写入位置 */

    /* 不设终止符（snprintf 已设置） */
}


/* 以下为 v2.0 新增 函数 */

/* v2.0 停用词列表 (NULL结尾，免数数量) */
static const char *stopwords[] = { /* 定义一个静态常量字符串指针数组，用于存储停用词 */
    /* 小写（默认模式 + -c 模式通用） */
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
    /* 大写（仅 -c 大小写敏感模式生效，默认模式 tolower 后冗余无害） */
    "A","About","Above","After","Again","Against","All","Am","An","And",
    "Any","Are","Aren't","As","At","Be","Because","Been","Before","Being",
    "Below","Between","Both","But","By","Can't","Cannot","Could","Couldn't",
    "Did","Didn't","Do","Does","Doesn't","Doing","Don't","Down","During","Each",
    "Few","For","From","Further","Had","Hadn't","Has","Hasn't","Have","Haven't",
    "Having","He","He'd","He'll","He's","Her","Here","Here's","Hers","Herself",
    "Him","Himself","His","How","How's","I","I'd","I'll","I'm","I've",
    "If","In","Into","Is","Isn't","It","It's","Its","Itself","Let's",
    "Me","More","Most","Mustn't","My","Myself","No","Nor","Not","Of",
    "Off","On","Once","Only","Or","Other","Ought","Our","Ours","Ourselves",
    "Out","Over","Own","Same","Shan't","She","She'd","She'll","She's",
    "Should","Shouldn't","So","Some","Such","Than","That","That's","The",
    "Their","Theirs","Them","Themselves","Then","There","There's","These",
    "They","They'd","They'll","They're","They've","This","Those","Through",
    "To","Too","Under","Until","Up","Very","Was","Wasn't","We","We'd",
    "We'll","We're","We've","Were","Weren't","What","What's","When",
    "When's","Where","Where's","Which","While","Who","Who's","Whom",
    "Why","Why's","With","Won't","Would","Wouldn't","You","You'd",
    "You'll","You're","You've","Your","Yours","Yourself","Yourselves",
    NULL  /* v2.0 哨兵，标志数组结束 */
};

/* v2.0 is_numeric_word — 检查单词是否为数字（纯数字或类似 "12-24"） */
int is_numeric_word(const char *word) {
    if (!word || !*word) return 0; /* 如果传入的字符串指针为 NULL 或者指向的字符串为空，则返回 0，表示不是数字词 */

     /* 检查每个字符是否为数字或连字符 */
    /* 首字符为数字即视为数字词 */
    return (word[0] >= '0' && word[0] <= '9');
}

/* v2.0 is_stopword — 检查单词是否为停用词 */
int is_stopword(const char *word) {
    for (int i = 0; stopwords[i] != NULL; i++) /* 遍历停用词列表，使用 strcmp 函数比较传入的单词与停用词列表中的每个词，如果找到匹配则返回 1，表示这个单词是停用词 */
        if (strcmp(word, stopwords[i]) == 0)
            return 1;
    return 0;
}

/* v2.0 calc_ttr — 计算词汇多样性 */
double calc_ttr(int unique, int total) {
    if (total <= 0) return 0.0;
    return ((double)unique / total) * 100.0;
}

/* v2.0 count_chars — 统计文件字符数 */
void count_chars(const char *file_path, int *chars_with_spaces,
                 int *chars_without_spaces) { /* 定义一个函数来统计文件中的字符数，接受一个文件路径字符串和两个整数指针作为参数，函数会打开指定的文件并逐字符读取内容，统计包含空格的字符数和不包含空格的字符数，并将结果通过指针参数返回 */
    *chars_with_spaces = 0; 
    *chars_without_spaces = 0;
    FILE *fp = fopen(file_path, "r");
    if (!fp) return;
    int c;
    while ((c = fgetc(fp)) != EOF) { /* 使用 fgetc 函数逐字符读取文件内容，直到文件结束（EOF），每次读取一个字符并存储在变量 c 中 */
        (*chars_with_spaces)++;
        if (c != ' ' && c != '\t' && c != '\n' && c != '\r') /* 如果读取的字符不是空格、制表符、换行符或回车符，则将不包含空格的字符数加 1，表示这是一个有效的非空白字符 */
            (*chars_without_spaces)++;
    }
    fclose(fp);
}

/* v2.0 cmp_by_alpha — qsort 比较函数（字母序）
 * 先按首字母 A→Z，同字母按频率降序 */
int cmp_by_alpha(const void *a, const void *b) { /* 定义一个比较函数，用于在使用 qsort 函数对 WordPair 结构体数组进行排序时，按照字母序进行比较，首先比较单词的首字母，如果首字母不同则按照字母顺序排序，如果首字母相同则按照频率降序排序，如果频率也相同则按照整个单词的字母序进行兜底排序 */
    const WordPair *pa = (const WordPair *)a;
    const WordPair *pb = (const WordPair *)b;
    char ca = pa->word[0]; /* 获取第一个单词的首字母 */
    char cb = pb->word[0]; /* 获取第二个单词的首字母 */
    if (ca != cb) return ca - cb;          /* 首字母 A→Z */
    /* 同字母按频率降序，频率相同则按字母序兜底 */
    if (pb->count != pa->count)
        return pb->count - pa->count; /* 同字母按频率降序，频率不同则返回频率差值，确保频率较高的单词排在前面 */
    return strcmp(pa->word, pb->word); /* 最后按整个单词的字母序进行比较，确保排序的稳定性和一致性 */
}

/* v2.0 output_html — 生成 HTML 格式报告 */
void output_html(WordPair *sorted, int unique_count, int total_words, /* 定义一个函数来生成 HTML 格式的报告，接受多个参数，包括排序后的单词数组、唯一单词数、总单词数 */
                 int max_count, int show_pct, /* 最高频率、是否显示百分比的标志 */
                 int chars_with_spaces, int chars_without_spaces, /* 字符统计数据 */
                 double ttr, const char *output_path) { /* 还接受多样性指数ttr、一个输出文件路径字符串，函数会根据这些参数生成一个包含单词频率统计信息的 HTML 文件，并将结果保存到指定的路径中 */
    FILE *fp = fopen(output_path, "w");
    if (!fp) return;

    fprintf(fp, "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n"); /* 写入 HTML 文件的头部信息，包括文档类型声明、HTML 标签、语言属性、字符集声明、标题head 和内嵌 CSS 样式，确保生成的 HTML 文件具有良好的结构和样式 */
    fprintf(fp, "<meta charset=\"UTF-8\">\n"); /* 设置字符编码为 UTF-8，确保文件能够正确显示各种字符，包括英文和特殊符号 */
    fprintf(fp, "<title>Word Frequency Report</title>\n"); /* 设置 HTML 文档的标题，在浏览器标签页中显示为 "Word Frequency Report" */
    fprintf(fp, "<style>\n"); /* 内嵌 CSS 样式定义，设置页面的字体、背景颜色、文本颜色、标题样式、摘要框样式、表格样式、柱状图样式等，确保生成的报告具有美观和易读的视觉效果 */
    fprintf(fp, "body{font-family:Arial,sans-serif;margin:40px;background:#0f0f14;color:#e4e4ec}\n"); /* 设置页面的字体为 Arial 或者 sans-serif，外边距为 40 像素，背景颜色为深色（#0f0f14），文本颜色为浅色（#e4e4ec），营造出暗色调的视觉效果 */
    fprintf(fp, "h1{color:#7c5cfc;border-bottom:2px solid #2a2a3a;padding-bottom:10px}\n"); /* 设置标题 h1 的颜色为紫色（#7c5cfc），下边框为 2 像素的实线，颜色为深灰色（#2a2a3a），下边距为 10 像素，增强标题的视觉突出效果 */
    fprintf(fp, ".summary{background:#1a1a24;padding:16px 24px;border-radius:8px;margin-bottom:24px}\n"); /* 定义一个名为 summary 的 CSS 类，用于样式化摘要信息的容器，设置背景颜色为深灰色（#1a1a24），内边距为 16 像素垂直和 24 像素水平，边框圆角为 8 像素，下外边距为 24 像素，使摘要信息在视觉上与其他内容区分开来并具有良好的可读性 */
    fprintf(fp, "table{width:100%%;border-collapse:collapse}\n"); /* 设置表格的宽度为 100%，并使用 border-collapse 属性将表格边框合并，确保表格的布局紧凑和整齐 */
    fprintf(fp, "th{background:#7c5cfc;color:#fff;padding:10px;text-align:left}\n"); /* 设置表头 th 的背景颜色为紫色（#7c5cfc），文本颜色为白色，内边距为 10 像素，文本左对齐，增强表头的视觉突出效果 */
    fprintf(fp, "td{padding:8px 10px;border-bottom:1px solid #2a2a3a}\n"); /* 设置表格单元格 td 的内边距为 8 像素垂直和 10 像素水平，底部边框为 1 像素的实线，颜色为深灰色（#2a2a3a），增强表格的可读性和视觉分隔效果 */
    fprintf(fp, "tr:hover{background:#1a1a24}\n"); /* 定义表格行 tr 的悬停效果，当鼠标悬停在表格行上时，背景颜色变为深灰色（#1a1a24），提供交互反馈，增强用户体验 */
    fprintf(fp, ".bar{background:#7c5cfc;height:16px;border-radius:3px;min-width:2px}\n"); /* 定义一个名为 bar 的 CSS 类，用于样式化柱状图的可视部分，设置背景颜色为紫色（#7c5cfc），高度为 16 像素，边框圆角为 3 像素，最小宽度为 2 像素，确保柱状图在视觉上具有良好的表现力和可见性 */
    fprintf(fp, "</style>\n</head>\n<body>\n"); /* 结束 head 部分，开始 body 部分，为后续的内容输出做好准备 */

     /* 输出报告标题和摘要信息，包括总单词数、唯一单词数、字符统计和词汇多样性指数等，使用 HTML 标签和 CSS 类进行格式化，使报告具有清晰的结构和良好的视觉效果 */

    fprintf(fp, "<h1>=== Word Frequency Report ===</h1>\n"); /* 输出报告的主标题，使用 h1 标签，并包含装饰性的等号，增强标题的视觉效果和突出性 */
    fprintf(fp, "<div class=\"summary\">\n"); /* 使用 div 标签和 summary CSS 类来包裹摘要信息，使其在视觉上与其他内容区分开来，并具有良好的可读性 */
    fprintf(fp, "<p>Total words: %d &nbsp;|&nbsp; Unique words: %d</p>\n", total_words, unique_count); /* 输出总单词数和唯一单词数，使用 p 标签进行段落格式化，并在数字之间添加适当的空格和分隔符，使信息清晰易读 */
    fprintf(fp, "<p>Chars (with spaces): %d &nbsp;|&nbsp; Chars (no spaces): %d</p>\n", chars_with_spaces, chars_without_spaces); /* 输出字符统计信息，包括含空格的字符数和不含空格的字符数，使用 p 标签进行段落格式化，并在数字之间添加适当的空格和分隔符，使信息清晰易读 */
    fprintf(fp, "<p>TTR (lexical diversity): %.1f%%</p>\n", ttr); /* 输出词汇多样性指数 TTR，使用 p 标签进行段落格式化，并将 TTR 值保留一位小数，增强信息的可读性和专业性 */
    fprintf(fp, "</div>\n"); /* 结束摘要信息的 div 标签 */

     /* 输出单词频率统计表格，包含单词、柱状图、计数和可选的百分比列，使用 HTML 表格标签进行结构化，并结合 CSS 样式进行美化，使统计信息清晰易读且具有良好的视觉效果 */

     /* 输出表头 */
     /* 使用 table 标签创建一个表格，tr 标签定义表格行，th 标签定义表头单元格，分别为 "Word"、"Bar"、"Count"，如果选择显示百分比则添加一个 "%%" 的表头，确保表格的结构清晰并且列标题明确 */

    fprintf(fp, "<table>\n<tr><th>Word</th><th>Bar</th><th>Count</th>"); /* 输出表格的表头行，包含 "Word"、"Bar" 和 "Count" 三列 */
    if (show_pct) fprintf(fp, "<th>%%</th>"); /* 如果 show_pct 标志为真，则在表头中添加一个 "%%" 列，表示百分比列，确保表格的结构清晰并且列标题明确 */
    fprintf(fp, "</tr>\n");

    int bar_max_px = 300; /* 定义一个整数变量 bar_max_px 来设置柱状图的最大宽度，单位为像素，这个值将用于计算每个单词对应的柱状图宽度，确保柱状图在视觉上具有适当的比例和可见性 */
    for (int i = 0; i < unique_count; i++) { 
        double pct = calc_percentage(sorted[i].count, total_words); /* 调用 calc_percentage 函数计算当前单词出现次数占总单词数的百分比，结果存储在 pct 变量中，供后续格式化输出使用 */
        int bar_w = 0; /* 定义一个整数变量 bar_w 来存储当前单词对应的柱状图宽度 */
        if (max_count > 0) /* 如果 max_count 大于 0，则根据当前单词的出现次数与最高频率的比例计算柱状图宽度，确保柱状图在视觉上具有适当的比例和可见性 */
            bar_w = (int)(((double)sorted[i].count / max_count) * bar_max_px); /* 根据当前单词的出现次数与最高频率的比例计算柱状图宽度 */
        if (bar_w < 2 && sorted[i].count > 0) bar_w = 2; /* 如果计算得到的柱状图宽度小于 2 像素且当前单词的出现次数大于 0，则将柱状图宽度设置为 2 像素，确保即使是频率较低的单词也能在视觉上有所表现，避免柱状图过于细小而难以看清 */

        fprintf(fp, "<tr><td>%s</td><td><div class=\"bar\" style=\"width:%dpx\"></div></td><td>%d</td>", /* 输出表格的每一行，包含单词、柱状图和计数 */
                sorted[i].word, bar_w, sorted[i].count);
        if (show_pct) fprintf(fp, "<td>%.1f%%</td>", pct); /* 如果 show_pct 标志为真，则在当前行中添加一个单元格显示百分比，格式化字符串 " %.1f%%" 表示先输出一个浮点数（pct）保留一位小数，并在数字后添加一个百分号 */
        fprintf(fp, "</tr>\n"); /* 结束当前行的 tr 标签，准备输出下一行，确保表格的结构完整和正确 */
    }

    fprintf(fp, "</table>\n</body>\n</html>\n"); /* 结束表格标签，结束 body 标签，结束 html 标签，完成 HTML 文件的结构，确保生成的 HTML 文件具有良好的格式和结构 */

     /* 关闭文件 */
    fclose(fp);
}
