#include <stdio.h>   // fprintf, fgets, fopen, fclose, fscanf, stderr标准错误输出流, stdin标准输入
#include <stdlib.h>  // qsort(快速排序算法), free(释放动态分配的内存)
#include <string.h>  // strcpy(字符串复制), strcspn(查找字符串中首个匹配字符集的字符位置，常用于去除换行符) 
#include <ctype.h>   // 引入字符类型库。提供：tolower(将大写字母转换为小写字母)
#include "hash_table.h" // 自定义哈希表头文件，包含 HashTable 结构体定义和相关函数声明

#define MAX_WORD_LEN 256   // 单个单词最大读取长度为256个字符（包含末尾的'\0'结束符）
#define MAX_PATH_LEN 512   // 文件路径最大长度为512个字符


// static (静态)：这是一个特殊标签。它意味着这个函数只能在当前这个 .c 文件里使用
static void str_tolower(char *str) {  // str_tolower 将字符串原地转为全小写
    for (int i = 0; str[i]; i++)  // 当前字符不为 '\0' 时继续循环
        str[i] = (char)tolower((unsigned char)str[i]);  // 需要先转为 unsigned char，避免负数传入导致未定义行为
        // tolower 函数规定，传入参数必须是能用 unsigned char（无符号字符型）表示的正数，或者是 EOF（-1）
}


 /* 两种运行方式：
  * 1. 命令行：`main.exe sample.txt`
  * 2. 交互式：利用code runner插件，弹出窗口后输入文件路径 
  */
int main(int argc, char *argv[]) {  // argc ：代表命令行参数的数量  argv ：是一个字符串数组，存着具体的参数

    /* 1. 获取文件路径
     * 用到：<string.h> strcpy（复制命令行参数）
     *       <stdio.h> printf, fgets（提示用户输入，读取一行）
     *       <string.h> strcspn（查找并截断换行符 \r 或 \n）
     */
    char file_path[MAX_PATH_LEN];
    if (argc >= 2) {
        strcpy(file_path, argv[1]);  // 命令行提供了路径，直接复制到 file_path 变量
    } else {
        printf("Enter file path: ");  // 提示用户输入
        fgets(file_path, sizeof(file_path), stdin);  // sizeof(file_path) 限制了读取长度  stdin 读取输入，包含换行符
        file_path[strcspn(file_path, "\r\n")] = '\0';  // 截掉换行符
    }

    /* 2. 打开文件
     * 用到：<stdio.h> fopen（以只读文本模式打开文件）
     * FILE 是 C 语言标准库定义的一个结构体，代表一个文件流。fopen 返回一个指向 FILE 结构体的指针，如果打开失败则返回 NULL。
     */
    FILE *fp = fopen(file_path, "r");
    if (!fp) {
        fprintf(stderr, "Error: Could not open file %s\n", file_path);
        return 1;  // 非零返回值通常表示程序异常退出，这里用 1 表示文件打开失败
    }

    /* 3. 读取并统计单词
     * 用到：hash_table.h 的 ht_create、ht_insert（哈希表操作）
     *       <stdio.h> fscanf 的 %s 格式（按空白符切分单词，自动跳过空格/换行）
     *       <ctype.h> tolower（大小写统一）
     */
    HashTable *ht = ht_create();  // ht_create()：在内存里开辟一块空间，初始化一个新的哈希表
    char buffer[MAX_WORD_LEN];  // buffer (缓冲区) 用来存储从文件中读取的单词并进行处理
    int total_words = 0;  // 统计总单词数的变量，初始值为0

    /* fscanf 从文件指针 fp 指向的文件里读取内容，放到 buffer 中
     * %s 会自动跳过前导空白符（空格、制表符、换行符等），并在遇到下一个空白符时停止读取
     * %255s 表示最多读取255个字符，最后一个字符留给 '\0' 结束符
     * fscanf 返回值为成功读取的项数，这里期望每次读取一个单词，如果成功读取一个单词则返回1，如果读到了文件末尾（EOF），返回-1，表示文件结束或发生错误
     */
    while (fscanf(fp, "%255s", buffer) == 1) {
        str_tolower(buffer);  // 调用str_tolower函数将单词转为小写
        ht_insert(ht, buffer);  // 插入哈希表，如果单词已存在则更新计数，否则添加新单词
        total_words++;  // 不管这个词是不是重复的，只要读到一个词，总数就加 1
    }
    fclose(fp);  // 关闭文件，释放系统资源

    /* 4. 排序
     * 用到：hash_table.h 的 ht_to_array（哈希表→数组）、cmp_by_freq（比较函数）
     *       <stdlib.h> qsort（标准库快速排序）
     */
    int unique_count;  // 这是一个空变量，专门用来接收“到底有多少个不重复的单词”
    WordPair *sorted = ht_to_array(ht, &unique_count);  
    // WordPair 是一个结构体，sorted 是一个指针，指向一个动态分配的数组，数组里每个元素都是一个 WordPair 结构体，存着一个不重复的单词和它的计数
    // ht_to_array 会返回一个动态分配的数组，里面存着所有不重复的单词和它们的计数，同时通过 unique_count 输出不重复单词的数量。
    qsort(sorted, unique_count, sizeof(WordPair), cmp_by_freq);  // 快速排序函数
    // sorted：待排序数组的首地址, unique_count：数组元素的数量
    // sizeof(WordPair)：每个元素的大小（占多少字节）, cmp_by_freq ：比较函数，定义了排序的规则（按频率从高到低）

    /* 5. 输出
     * 用到：<stdio.h> printf
     */
    int top_n = unique_count;  // 这里我们输出所有不重复的单词，如果只想输出前 N 个，可以把 top_n 设置为一个固定值，比如 10，也可以用三目运算符
    printf("All %d unique words (sorted by frequency):\n", top_n);
    for (int i = 0; i < top_n; i++)
        printf("'%s': %d\n", sorted[i].word, sorted[i].count);  
        // sorted[i].word：访问第 i 个单词，word 是一个指针，指向哈希表内部的字符串
        // sorted[i].count：访问该单词对应的出现次数
    printf("Total words: %d\n", total_words);

    /* 6. 释放内存
     * 用到：hash_table.h 的 ht_destroy（释放哈希表）
     *       <stdlib.h> free（释放排序数组）
     * 顺序：先释放排序数组sorted（只存指针），再释放哈希表（释放实际字符串）因为排序数组里的 word 指针指向哈希表内部的字符串，必须先释放排序数组，避免悬空指针
     */
    free(sorted);
    ht_destroy(ht);
    return 0;
}
