1.0版功能说明：
- 读取本地文本文件
- 按空白字符（空格、换行、制表符）简单分词
- 统计每个单词出现次数，输出Top 5高频词及出现次数
- 输出文章总单词数
- 命令行用法：wordTally <file_path>
- 代码结构：单文件main.cpp，所有逻辑内联

1.1版更新说明：
- 拆分模块：main.cpp + word_analyzer.h + word_analyzer.cpp
- 分词优化：去除标点符号，分词更准确
- 新增排除词过滤功能（-e word1 word2 ...）
- 新增词长过滤功能（--min-length / --max-length）
- 新增自定义Top N数量（-t N，默认5）
- 新增百分比显示开关（-p）
- 新增结果输出到文件（-o output.txt）
- 所有单词统一转小写，不区分大小写
- 增加基础错误处理（文件不存在等）
- 命令行用法：wordTally -l <file> [-e words...] [-t N] [--min-length N] [--max-length N] [-p] [-o <output>]

2.0版完整功能说明：
- 拆分模块：main.cpp + cli_parser.h/.cpp + word_analyzer.h/.cpp + CMakeLists.txt
- 新增总字符数统计（含空格/不含空格）
- 新增词汇多样性指标（不重复词数 / 总词数）
- 新增词长分布直方图（文本形式输出各长度区间的单词数量）
- 新增两级排序模式（--sort alpha）：先按首字母A->Z，同字母内按单词长度由短到长；默认按频率降序（--sort freq）
- 新增排除词文件支持（-x stopwords.txt），从文件加载排除词列表
- 新增大小写敏感开关（-c）：开启后区分大小写，大写字母排序优先于小写
- 完善错误处理：空文件、文件编码异常、大文件等情况
- 使用CMake构建系统
- 命令行用法：wordTally -l <file> [-e words...] [-x <stopwords>] [-t N] [--min-length N] [--max-length N] [-p] [-c] [--sort freq|alpha] [-o <output>]
