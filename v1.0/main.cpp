#include <iostream>  // 输入输出流：std::cout（打印到屏幕）、std::cerr（打印错误信息）、std::endl（换行）
#include <fstream>   // 文件流：std::ifstream（打开并读取文件内容）
#include <string>    // 字符串类型：std::string（存储每个单词、文件路径等文本数据）
#include <unordered_map>  // 哈希表（无序映射）：std::unordered_map<单词, 次数>，用于快速统计每个单词出现了多少次
#include <vector>    // 动态数组：std::vector，用于将哈希表中的统计结果转存为可排序的数组
#include <algorithm> // 算法库：std::sort（排序）、std::min（取较小值）
#include <cctype>    // 字符处理：std::tolower（将大写字母转小写）、static_cast（类型转换）

/* main 函数：程序入口点
 * 参数 argc：命令行参数个数（包含程序名本身）
 * 参数 argv：命令行参数字符串数组，argv[0] 是程序名，argv[1] 是用户指定的文件路径
 */
int main(int argc, char* argv[]) {

    // ----- 1. 获取文件路径 -----
    // 用到：<iostream> + <string>
    // 如果命令行提供了路径就用，否则交互式询问用户输入
    std::string file_path;
    if (argc >= 2) {
        file_path = argv[1];               // 命令行方式：wordTally sample.txt
    } else {
        std::cout << "Enter file path: ";  // 交互方式：三角按钮直接运行
        std::getline(std::cin, file_path); // 等待用户输入路径后按回车
    }

    // ----- 2. 打开文件 -----
    // 用到：<string> 的 std::string（存储路径）、<fstream> 的 std::ifstream（打开文件）
    std::ifstream file(file_path);         // 创建文件输入流对象，尝试打开文件
    if (!file.is_open()) {                 // is_open() 检查文件是否成功打开
        std::cerr << "Error: Could not open file " << file_path << std::endl;
        return 1;
    }

    // ----- 3. 读取并统计单词 -----
    // 用到：<unordered_map> 创建 单词→次数 的映射表
    //       <string> 的 std::string 存储当前读到的单词
    //       <cctype> 的 std::tolower 将字符转小写
    //       <fstream> 的 >> 运算符按空白字符（空格/换行/制表符）逐个提取单词
    std::unordered_map<std::string, int> word_count;  // key=单词, value=出现次数
    std::string word;                                  // 临时变量，存储每次读到的单词
    int total_words = 0;                               // 总单词数计数器（含重复）

    while (file >> word) {                     // >> 运算符自动跳过空白字符，每次读一个"单词"
                                               // 注意：标点符号不会被去除，"hello," 和 "hello" 会被当成两个不同的词
        // 将当前单词的每个字符转为小写（实现不区分大小写统计）
        for (char& c : word) {
            c = std::tolower(static_cast<unsigned char>(c));
            // static_cast<unsigned char>：确保字符值在0~255范围，避免负数传入tolower导致未定义行为
        }
        word_count[word]++;  // 哈希表操作：如果word不存在则自动插入并初始化为0，然后+1
        total_words++;       // 每读到一个单词，总数+1
    }

    file.close();  // 读取完毕，关闭文件

    // ----- 4. 排序 -----
    // 用到：<vector> 创建可排序的动态数组
    //       <algorithm> 的 std::sort 按指定规则排序
    // unordered_map 本身无序，需要转为 vector 后才能排序
    std::vector<std::pair<std::string, int>> sorted_words(
        word_count.begin(), word_count.end()  // 将哈希表的所有键值对拷贝到vector中
    );
    std::sort(sorted_words.begin(), sorted_words.end(),
        // Lambda表达式：自定义排序规则——按出现次数（second）从大到小
        [](const auto& a, const auto& b) {
            return a.second > b.second;       // a的次数 > b的次数 → true → a排在b前面
        }
    );

    // ----- 5. 输出结果 -----
    // 用到：<iostream> 的 std::cout 打印到屏幕
    //       <algorithm> 的 std::min 确保输出的数量不超过实际单词种类数
    int top_n = std::min(5, static_cast<int>(sorted_words.size()));
    // 如果文本中不同单词少于5个，就只输出实际数量

    std::cout << "Top " << top_n << " most frequent words:" << std::endl;
    for (int i = 0; i < top_n; i++) {
        std::cout << "'" << sorted_words[i].first   // 单词（键）
                  << "': " << sorted_words[i].second // 次数（值）
                  << std::endl;
    }

    std::cout << "Total words: " << total_words << std::endl;

    return 0;  // 返回0表示程序正常结束
}
