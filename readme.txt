══════════════════════════════════════════════════════════════
  文本单词频率统计工具 — Word Frequency Statistics Tool
  程序语言与数据结构 课程设计
══════════════════════════════════════════════════════════════

  作者　：周畅
  学号　：202521093148
  语言　：C（C11 标准，-std=c11 -mconsole）
  编译器：gcc 4.9.2 (MinGW, Dev-Cpp 自带)
  依赖　：纯标准库，无第三方依赖
  参考　：RemmyNine/WordTally (Python)

══════════════════════════════════════════════════════════════
  目录结构
══════════════════════════════════════════════════════════════

  工程项目/
  +-- readme.txt          <- 本文件（三版本总览）
  +-- v1.0.dev             Dev-Cpp 项目文件（v1.0）
  +-- v1.1.dev             Dev-Cpp 项目文件（v1.1）
  +-- v2.0.dev             Dev-Cpp 项目文件（v2.0）
  +-- v1.0/               基础原型
  |   +-- main.c         主程序（单文件 + 哈希表模块）
  |   +-- hash_table.h   哈希表头文件
  |   +-- hash_table.c   哈希表实现
  |   +-- build.bat      编译脚本
  |   +-- sample.txt     示例文本
  +-- v1.1/               功能扩展
  |   +-- main.c         主程序（双模式）
  |   +-- hash_table.h/.c 哈希表（复用 v1.0）
  |   +-- word_utils.h/.c 单词清洗 / 长度过滤 / 百分比 / 柱状图
  |   +-- cli_parser.h/.c 命令行参数解析
  |   +-- build.bat       编译脚本
  |   +-- sample.txt      示例文本
  +-- v2.0/               最终完善
      +-- main.c         主程序（11步交互，全功能双模式）
      +-- hash_table.h/.c 哈希表（复用 v1.0）
      +-- word_utils.h/.c 停用词 / TTR / 字符统计 / alpha排序 / HTML
      +-- cli_parser.h/.c 命令行解析（+ -c/-s/--sort/--html）
      +-- build.bat       编译脚本
      +-- sample.txt      示例文本

══════════════════════════════════════════════════════════════
  版本演进总览
══════════════════════════════════════════════════════════════

  v1.0  [完成]  哈希表 + 基础词频统计
  v1.1  [完成]  功能扩展 + 双模式 + 可视化
  v2.0  [完成]  字符统计 + 词汇多样性 + 多维排序 + HTML 输出


══════════════════════════════════════════════════════════════
  v1.0 — 基础原型
══════════════════════════════════════════════════════════════

  功能
  - 读取本地文本文件，按空白字符分词
  - 大小写不敏感统计（读取时自动转小写）
  - 按词频降序输出全部单词及其出现次数
  - 支持命令行传参或交互式输入文件路径

  核心数据结构
  手写哈希表（djb2 哈希函数 + 链地址法，桶数 10007 质数）
  排序：将哈希表导出为数组，qsort 按频率降序排列

  用法
  > main.exe sample.txt             命令行传入文件路径
  > main.exe                        （无参数）交互式提示输入

  文件清单
  main.c  hash_table.h  hash_table.c  build.bat  sample.txt

  编译
  gcc -std=c11 -mconsole -o main.exe main.c hash_table.c


══════════════════════════════════════════════════════════════
  v1.1 — 功能扩展
══════════════════════════════════════════════════════════════

  新增功能
  - 标点符号清洗：去除单词首尾标点（.,!?;:'"等），
    保留词中连字符 (-) 和撇号 (')，过滤全标点词
  - 排除指定单词：-e 选项 / 交互式引导
  - 单词长度过滤：--min-length / --max-length
  - 自定义 Top N 显示：-t 选项 / 交互式引导（告知上限）
  - 百分比显示：-p 选项 / 交互式默认开启
  - ASCII 柱状图：用 # 字符呈现词频对比（兼容所有终端编码）
  - 输出到文件：-o 选项 / 交互式必填（默认 wordfreq_result.txt）
  - 交互式引导模式：7 步向导，适合无命令行经验的用户

  双模式对比

    文件路径
      命令行：-f <file>
      交互式：第 1 步输入

    排除词
      命令行：-e word1 word2 ...
      交互式：第 2 步输入（空格分隔，回车跳过）

    输出文件
      命令行：-o <file>
      交互式：第 3 步输入（回车 = 默认 wordfreq_result.txt）

    Top N
      命令行：-t <N>
      交互式：第 4 步选择（提示上限）

    百分比
      命令行：-p
      交互式：第 5 步确认（默认 Y）

    长度过滤
      命令行：--min-length / --max-length
      交互式：[不支持]

  注：交互模式不支持长度过滤是刻意设计——保持交互流程简洁，
  高级过滤功能仅面向命令行用户。

  交互式模式流程
  1. 输入文件路径
  2. 输入排除词（空格分隔，回车 = 不排除）
  3. 输入输出文件名（回车 = 默认 wordfreq_result.txt）
  4. [处理文件，显示总词数和不重复词数]
  5. Top N（0 或回车 = 全部，提示实际上限如 max 11）
  6. 百分比（Y/n，默认 Y）
  7. 输出柱状图结果（屏幕 + 文件）

  命令行选项
  -f <file>              输入文本文件（必需）
  -e <word1> <word2>...  排除指定单词（空格分隔）
  -t <N>                 显示前 N 个单词（默认所有）
  --min-length <N>       最小单词长度过滤
  --max-length <N>       最大单词长度过滤
  -p                     显示百分比
  -o <file>              输出结果到文件

  使用示例
  > main.exe -f sample.txt -t 10 -p -o result.txt
  > main.exe -f sample.txt -e a an the --min-length 3 -t 5

  代码结构
  语言：C（C11 标准），总计约 750 行
  文件：main.c + 3 个模块（6 个 .h/.c 文件）

  - hash_table.h/.c   哈希表（djb2 + 链地址法，复用 v1.0）
  - word_utils.h/.c   标点清洗、长度过滤、百分比计算、柱状图绘制
  - cli_parser.h/.c   命令行参数解析
  - main.c            主流程：解析 -> 统计 -> 排序 -> 输出，双模式调度

  编译
  > gcc -std=c11 -mconsole -o main.exe main.c hash_table.c word_utils.c cli_parser.c
  或直接运行 build.bat（显式列出源文件，兼容 gcc 4.9.2）

  v1.0 -> v1.1 主要变更
  - 新增 word_utils 模块：洗标点 -> "world!" 和 "world" 现在视为同一词
  - 新增 cli_parser 模块：完整的命令行选项支持
  - 新增 ASCII 柱状图：用 # 字符呈现频率对比（避免 UTF-8 编码问题）
  - 新增百分比和文件输出功能
  - 新增交互式引导模式：逐步提示输入，最终输出到文件
  - 构建脚本化：build.bat 替代 Code Runner 内联命令，支持多文件编译


══════════════════════════════════════════════════════════════
  v2.0 — 最终完善
══════════════════════════════════════════════════════════════

  新增功能
  - 字符统计：含空格 / 不含空格字符数（自动显示）
  - 词汇多样性 TTR：不重复词数 ÷ 总词数 × 100%（自动显示）
  - 大小写敏感开关：-c / 交互式第 3 步（默认不敏感）
  - 停用词过滤：-s / 交互式第 4 步，内置 ~176 常用英文停用词
  - 多维排序 --sort：freq（频率降序）| alpha（首字母 A→Z，同字母按频率降序，同频率按字母序兜底）
  - HTML 报告输出：--html / 交互式第 5 步，深色主题彩色柱状图
  - 数字词过滤：自动跳过纯数字或以数字开头的"词"（如 "12"、"1st"）
  - 11 步交互式引导：覆盖全部功能，每步告知默认值/上限
  - 版本标记：[v1.0]/[v1.1]/[v2.0] 标注代码迭代关系

  双模式对比（v2.0 新增项加 *）

    文件路径          -f <file>                  交互式第 1 步
    排除词            -e word1 word2 ...         交互式第 2 步
    大小写敏感*       -c                         交互式第 3 步
    停用词过滤*       -s                         交互式第 4 步
    输出格式*         --html（HTML）/ 默认文本    交互式第 5 步
    输出文件           -o <file>                  交互式第 6 步
    Top N             -t <N>                     交互式第 7 步（告知上限）
    百分比             -p                         交互式第 8 步（默认 Y）
    排序方式*          --sort freq|alpha          交互式第 9 步
    长度过滤           --min/--max-length         交互式 [不支持]

  交互式模式流程（11 步）
  1.  输入文件路径
  2.  输入排除词（空格分隔，回车跳过）
  3.  大小写敏感（y/N，默认 N）
  4.  启用停用词过滤（y/N，默认 N）
  5.  输出格式（Text/HTML，默认 Text）
  6.  输出文件名（回车 = 默认 wordfreq_result.txt / wordfreq_report.html）
  7.  [处理文件，显示总词数、不重复词数、字符统计、TTR]
  8.  Top N（0 = 全部，提示实际上限）
  9.  百分比（Y/n，默认 Y）
  10. 排序方式（freq/alpha，默认 freq）
  11. 输出结果（屏幕 + 文件）

  命令行选项
  -f <file>              输入文本文件（必需）
  -e <word1> <word2>...  排除指定单词（空格分隔）
  -t <N>                 显示前 N 个单词（默认所有）
  --min-length <N>       最小单词长度过滤
  --max-length <N>       最大单词长度过滤
  -p                     显示百分比
  -o <file>              输出结果到文件
  -c                     大小写敏感（默认：不敏感）
  -s                     启用停用词过滤
  --sort <freq|alpha>    排序方式（默认：freq 降序）
  --html                 HTML 格式输出（默认：文本）

  使用示例
  > main.exe -f sample.txt -t 10 -p -s
  > main.exe -f sample.txt -c --sort alpha -t 20 -p
  > main.exe -f sample.txt -t 10 -p -s -o result.html --html

  代码结构
  语言：C（C11 标准），总计约 1100 行
  所有代码文件中标注了每个功能的引入版本，老师可直接看到迭代关系

  - hash_table.h/.c   哈希表 [v1.0]，v1.1/v2.0 完全复用
  - word_utils.h/.c   [v1.1 新增] [v2.0 扩展] 停用词/TTR/字符统计/alpha排序/HTML
  - cli_parser.h/.c   [v1.1 新增] [v2.0 扩展] -c/-s/--sort/--html
  - main.c            主流程 [v1.0→v1.1→v2.0] 11步交互 + 双模式全功能

  编译
  > gcc -std=c11 -mconsole -o main.exe main.c hash_table.c word_utils.c cli_parser.c
  或直接运行 build.bat（显式列出源文件，兼容 gcc 4.9.2）

  v1.1 -> v2.0 主要变更
  - 新增字符统计和 TTR 词汇多样性指标
  - 新增停用词过滤（~176 词）和数字词自动过滤
  - 新增 --sort alpha 多维排序（首字母→频率→字母序）
  - 新增 HTML 深色主题报告输出
  - 新增大小写敏感开关
  - 交互模式从 7 步扩展到 11 步，全功能覆盖
  - 代码添加 [v1.0]/[v1.1]/[v2.0] 版本标记


══════════════════════════════════════════════════════════════
  Dev-Cpp 验收指南
══════════════════════════════════════════════════════════════

  打开项目
  1. 运行 D:\Dev-Cpp\devcppPortable.exe
  2. 文件 → 打开 → 选择 v1.0.dev / v1.1.dev / v2.0.dev

  编译设置（仅首次）
  运行 → 编译选项 → 编译器标签 →
  勾选"编译时加入以下命令" → 填入：-std=c11 -mconsole

  运行
  - 交互模式：清空运行参数，直接点运行
  - 命令行模式：运行 → 参数 → 填入 -f sample.txt -t 10 -p

  记事本查看输出
  格式 → 字体 → 选择 Consolas 等宽字体（否则柱状图错位）


══════════════════════════════════════════════════════════════
  构建与环境
══════════════════════════════════════════════════════════════

  编译器
  gcc 4.9.2 (tdm64-1)，Dev-Cpp 自带 MinGW，2014 年发布
  已知限制：不支持 *.c 通配符展开

  构建方式
  每个版本目录下执行 build.bat：
  > cd v1.1
  > build.bat
  build.bat 显式列出所有 .c 源文件，解决 gcc 4.9.2 通配符不支持问题

  VS Code 集成
  - Code Runner（运行）：settings.json 调用 build.bat
  - Ctrl+Shift+B（构建）：tasks.json 调用 gcc -std=c11 -g
  - F5（调试）：cppdbg + gdb，默认传入 sample.txt


══════════════════════════════════════════════════════════════
  关键问题记录
══════════════════════════════════════════════════════════════

  1. *.c 通配符编译失败
     原因：Windows cmd 不展开通配符，gcc 4.9.2 不支持 glob
     解决：每个版本用 build.bat 显式列出源文件

  2. strtok() 导致程序崩溃（exit code 5）
     原因：gcc 4.9.2 环境下 strtok 在特定输入场景有兼容性问题
     解决：改用手动逐字符扫描切分空格

  3. 柱状图乱码（UTF-8 全角方块 → 鈻堚枅）
     原因：Windows cmd 默认 GBK，不支持 UTF-8 多字节字符
     解决：改用纯 ASCII 字符 # 绘制柱状图

  4. Code Runner 只编译 main.c + hash_table.c
     原因：settings.json 中硬编码了编译文件列表
     解决：改为调用 build.bat，每个版本独立维护编译文件列表

  5. Dev-Cpp 编译后双击 exe 无窗口
     原因：Dev-Cpp 默认链接选项 -mwindows 将 exe 标记为 GUI 子系统，
     Windows 不为其分配控制台窗口
     解决：编译时加 -mconsole 覆盖，强制分配控制台

  6. 交互模式 getchar() 不等待用户按键
     原因：交互输入后输入缓冲残留换行符，getchar() 直接读取跳过
     解决：getchar() 前调用 fflush(stdin) 清空输入缓冲

  7. 柱状图在记事本中错位
     原因：Win10/11 记事本默认字体为等宽以外字体，空格和 # 宽度不同
     解决：记事本 → 格式 → 字体 → 选择 Consolas 等宽字体

  8. VS Code → Dev-Cpp 迁移兼容性问题

     本项目先在 VS Code + Code Runner 中开发，验收时需迁移到
     Dev-Cpp 5.11（gcc 4.9.2）。迁移过程连续遇到多个连锁报错：

     ① 'for' loop initial declarations are only allowed in C99 or C11 mode
        原因：Dev-Cpp 默认使用 C89 标准编译，不允许 for (int i=0;...)
        解决：加编译选项 -std=c11

     ② i386 architecture of input file 'gcc.exe' is incompatible with
        i386:x86-64 output
        原因：在 .dev 项目文件中误填了 Compiler=、CppCompiler=、Linker=
        字段，Dev-Cpp 将其值当成额外编译链接参数，把 gcc.exe 本体当作
        目标文件丢给链接器
        解决：删除这三个字段，Dev-Cpp 自带编译器路径无需手动指定

     ③ cannot open output file v2.0/v2.0/main.exe
        原因：.dev 中 ExeOutput=v2.0\ 与 OverrideOutputName=v2.0\main.exe
        路径叠加导致嵌套
        解决：OverrideOutputName=main.exe（仅文件名，不含目录前缀）

     ④ 双击 exe 无任何窗口弹出（见第 5 条，Dev-Cpp 默认 -mwindows）
        解决：编译选项加 -mconsole

     ⑤ Makefile.win 缓存旧配置
        原因：Dev-Cpp 生成的 Makefile 不会随 .dev 修改自动刷新
        解决：每次修改 .dev 后，关闭 Dev-Cpp，删除 Makefile.win，重开项目
