/* cli_parser.h — 命令行参数解析声明
 * 功能：解析单词频率统计工具的命令行参数，将 argc/argv 转换为结构化的配置
 * v1.1 新增模块
 * v2.0 扩展 新增 -c -s --sort --html 四个 flag
*/

#ifndef CLI_PARSER_H
#define CLI_PARSER_H //防止重复包含

/* 排除词列表最大容量 */
#define MAX_EXCLUDE_WORDS 100

/* CliConfig — 命令行配置结构体
 * 所有字段均有默认值，未指定时保持默认
 */
typedef struct {
    char file_path[512];                /* 输入文件路径 (-f) */
    char output_path[512];              /* 输出文件路径 (-o)，空串 = 不输出到文件 */
    char exclude_words[MAX_EXCLUDE_WORDS][64];  /* 排除词列表 (-e) */
    int  exclude_count;                 /* 排除词数量 */
    int  top_n;                         /* 显示前 N 个 (-t)，默认显示全部 */
    int  min_length;                    /* 最小单词长度 (--min-length)，0 = 不限制 */
    int  max_length;                    /* 最大单词长度 (--max-length)，0 = 不限制 */
    int  show_percentage;               /* 是否显示百分比 (-p)，1=显示, 0=不显示 */
    int  has_error;                     /* 解析过程中是否有错误 ，1=有错误, 0=无错误 */

    /* v2.0 新增 */
    int  case_sensitive;                /* 大小写敏感 (-c)，0=不敏感(默认), 1=敏感 */
    int  use_stopwords;                 /* 启用停用词过滤 (-s)，0=关闭(默认), 1=启用 */
    int  sort_mode;                     /* 排序方式 (--sort)，0=freq降序(默认), 1=alpha */
    int  html_output;                   /* HTML 输出 (--html)，0=文本(默认), 1=HTML */
} CliConfig;

/* parse_cli — 解析命令行参数
 * 参数 argc, argv：main 函数的命令行参数
 * 返回：填充好的 CliConfig 结构体
 * 支持的参数：
 *   -f <file>           输入文件路径（必需）
 *   -e <word1> <word2>... 要排除的词
 *   -t <N>              显示前 N 个
 *   --min-length <N>    最小单词长度
 *   --max-length <N>    最大单词长度
 *   -p                  显示百分比
 *   -o <file>           输出文件路径
 *   v2.0 -c           大小写敏感
 *   v2.0 -s           启用停用词过滤
 *   v2.0 --sort <freq|alpha>  排序方式
 *   v2.0 --html       HTML 格式输出
 */
CliConfig parse_cli(int argc, char *argv[]);

/* print_usage — 打印使用说明 */
void print_usage(const char *program_name);

#endif /* CLI_PARSER_H */
