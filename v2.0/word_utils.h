/* word_utils.h — 单词处理工具声明
 * 功能：单词清洗、长度过滤、百分比计算、柱状图生成
 * [v2.0 扩展] 新增字符统计、TTR、停用词、alpha排序、HTML输出
 * 纯 C11 标准，无第三方依赖
 * v1.1 新增模块
*/

#ifndef WORD_UTILS_H
#define WORD_UTILS_H

#include "hash_table.h"  /* [v2.0] 引入 WordPair 类型 */

/* 柱状图最大宽度（字符数） */
#define BAR_MAX_WIDTH 40  /* [v2.0] 调窄避免终端换行 */

/* [v2.0] 停用词数量 */
#define STOPWORDS_COUNT 200

/* clean_word — 清洗单词的首尾标点符号
 * 功能：去除单词开头和结尾的标点（.,!?;:'\"()-[]{}等），
 *       保留单词中间的连字符和撇号（如 don't, state-of-the-art）
 *       清洗后若单词为空（如纯符号"!!!"），返回 NULL
 * 参数 word：原始单词字符串（原地修改，调用者负责缓冲区）
 * 返回    ：清洗后单词的首地址，若结果为空串则返回 NULL
 */
char *clean_word(char *word);

/* word_matches_length — 检查单词长度是否在指定范围内
 * 参数 word：单词字符串
 * 参数 min_len：最小长度（包含），0 表示不限制
 * 参数 max_len：最大长度（包含），-1 表示不限制
 * 返回    ：1 匹配，0 不匹配
 */
int word_matches_length(const char *word, int min_len, int max_len);

/* calc_percentage — 计算百分比
 * 参数 count：单词出现次数
 * 参数 total：总单词数
 * 返回    ：百分比（浮点数，如 25.50 表示 25.50%）
 */
double calc_percentage(int count, int total);

/* draw_bar — 绘制水平 ASCII 柱状图行
 * 功能：根据比例生成一条格式化的柱状图行
 *       格式："word  ########  12  (24.5%)"
 * 参数 word      ：单词字符串
 * 参数 count     ：出现次数
 * 参数 max_count ：最高频率（用于缩放）
 * 参数 total     ：总单词数（用于百分比计算）
 * 参数 show_pct  ：是否显示百分比（1=显示, 0=不显示）
 * 参数 buf       ：输出缓冲区（调用者提供）
 * 参数 buf_size  ：缓冲区大小
 */
void draw_bar(const char *word, int count, int max_count, int total,
              int show_pct, char *buf, int buf_size);

/* === [v2.0 新增] === */

/* is_stopword — 检查单词是否为停用词
 * 参数 word：要检查的单词
 * 返回    ：1 是停用词，0 不是
/* [v2.0] is_stopword — 检查单词是否为停用词 */
int is_stopword(const char *word);

/* [v2.0] is_numeric_word — 检查单词是否为数字 */
int is_numeric_word(const char *word);

/* calc_ttr — 计算词汇多样性 (Type-Token Ratio)
 * 参数 unique：不重复单词数
 * 参数 total ：总单词数
 * 返回    ：TTR 百分比
 */
double calc_ttr(int unique, int total);

/* count_chars — 统计文件的字符数
 * 参数 file_path：文件路径
 * 参数 chars_with_spaces   ：输出，含空格字符数
 * 参数 chars_without_spaces：输出，不含空格字符数
 */
void count_chars(const char *file_path, int *chars_with_spaces,
                 int *chars_without_spaces);

/* cmp_by_alpha — qsort 比较函数（字母序）
 * 先按首字母 A→Z 排序，同字母按词长升序
 */
int cmp_by_alpha(const void *a, const void *b);

/* has_non_ascii — 检查单词是否包含非 ASCII 字节（>= 0x80）
 * 英文词频统计中，含 em dash、弯引号等 UTF-8 多字节字符的
 * 单词应丢弃，避免 printf 按字节对齐时错位
 */
int has_non_ascii(const char *word);

/* output_html — 生成 HTML 格式报告
 * 参数 sorted：排序后的 WordPair 数组
 * 参数 unique_count：不重复单词数
 * 参数 total_words：总单词数
 * 参数 max_count：最高频率
 * 参数 show_pct：是否显示百分比
 * 参数 chars_with_spaces / chars_without_spaces：字符统计
 * 参数 ttr：词汇多样性
 * 参数 output_path：输出文件路径
 */
void output_html(WordPair *sorted, int unique_count, int total_words,
                 int max_count, int show_pct,
                 int chars_with_spaces, int chars_without_spaces,
                 double ttr, const char *output_path);

#endif /* WORD_UTILS_H */
