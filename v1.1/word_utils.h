/*  word_utils.h — 单词处理工具声明
 * 功能：单词清洗、长度过滤、百分比计算、柱状图生成
 * v1.1 新增模块
 */

#ifndef WORD_UTILS_H
#define WORD_UTILS_H

/* 柱状图最大宽度（字符数） */
#define BAR_MAX_WIDTH 50 //定义一个宏常量，表示柱状图的最大宽度为50个字符，这个值用于在生成柱状图时进行缩放，以确保最长的柱子占满预设的最大宽度

/* clean_word — 清洗单词的首尾标点符号
 * 功能：去除单词开头和结尾的标点（.,!?;:'\"()-[]{}等），
 * 保留单词中间的连字符和撇号（如 don't, state-of-the-art）
 * 清洗后若单词为空（如纯符号"!!!"），返回 NULL
 * 参数 word：原始单词字符串（原地修改，调用者负责缓冲区）
 * 返回：清洗后单词的首地址，若结果为空串则返回 NULL
*/
char *clean_word(char *word);

/* word_matches_length — 检查单词长度是否在指定范围内
 * 参数 word：单词字符串
 * 参数 min_len：最小长度（包含），0 表示不限制
 * 参数 max_len：最大长度（包含），-1 表示不限制
 * 返回：1 匹配，0 不匹配
*/
int word_matches_length(const char *word, int min_len, int max_len);

/* calc_percentage — 计算百分比
 * 参数 count：单词出现次数
 * 参数 total：总单词数
 * 返回：百分比（浮点数，如 25.50 表示 25.50%）
*/
double calc_percentage(int count, int total);

/* draw_bar — 绘制水平 ASCII 柱状图行
 * 功能：根据比例生成一条格式化的柱状图行
 * 格式："word  #####  12  (24.5%)"
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

#endif /* WORD_UTILS_H */
