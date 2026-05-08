#include <stdio.h>   // fprintf, fgets, fopen, fclose, fscanf, stderr标准错误输出流, stdin标准输入
#include <stdlib.h>  // qsort(快速排序算法), free(释放动态分配的内存)
#include <string.h>  // strcpy(字符串复制), strcspn(查找字符串中首个匹配字符集的字符位置，常用于去除换行符) 
#include <ctype.h>   // 引入字符类型库。提供：tolower(将大写字母转换为小写字母)
#include "hash_table.h" // 自定义哈希表头文件，包含 HashTable 结构体定义和相关函数声明

#define MAX_WORD_LEN 256   // 单个单词最大读取长度为256个字符（包含末尾的'\0'结束符）
#define MAX_PATH_LEN 512   // 文件路径最大长度为512个字符


// static (静态)：这是一个特殊标签。它意味着这个函数只能在当前这个 .c 文件里使用
static void str_tolower(char *str) { // str_tolower 将字符串原地转为全小写
    for (int i = 0; str[i]; i++) // 当前字符不为 '\0' 时继续循环
        str[i] = (char)tolower((unsigned char)str[i]); // 需要先转为 unsigned char，避免负数传入导致未定义行为
        // tolower 函数规定，传入参数必须是能用 unsigned char（无符号字符型）表示的正数，或者是 EOF（-1）
}

/*
 * main 程序入口
 * 两种运行方式：
 *  1. 命令行：`main.exe sample.txt`
 *  2. 交互式：直接双击或点三角形按钮，弹出窗口后输入文件路径
 */
int main(int argc, char *argv[]) {

    /* ----- 1. 获取文件路径 -----
     * 用到：<string.h> strcpy（复制命令行参数）
     *       <stdio.h> printf, fgets（提示用户输入，读取一行）
     *       <string.h> strcspn（查找并截断换行符 \r 或 \n）
     */
    char file_path[MAX_PATH_LEN];
    if (argc >= 2) {
        strcpy(file_path, argv[1]);          /* 命令行提供了路径 */
    } else {
        printf("Enter file path: ");         /* 提示用户输入 */
        fgets(file_path, sizeof(file_path), stdin);  // stdin 读取输入一行，包含换行符
        file_path[strcspn(file_path, "\r\n")] = '\0';  /* 截掉换行符 */
    }

    /* ----- 2. 打开文件 -----
     * 用到：<stdio.h> fopen（以只读文本模式打开文件）
     */
    FILE *fp = fopen(file_path, "r");
    if (!fp) {
        fprintf(stderr, "Error: Could not open file %s\n", file_path);
        return 1;
    }

    /* ----- 3. 读取并统计单词 -----
     * 用到：hash_table.h 的 ht_create、ht_insert（哈希表操作）
     *       <stdio.h> fscanf 的 %s 格式（按空白符切分单词，自动跳过空格/换行）
     *       <ctype.h> tolower（大小写统一）
     */
    HashTable *ht = ht_create();
    char buffer[MAX_WORD_LEN];
    int total_words = 0;

    while (fscanf(fp, "%255s", buffer) == 1) {
        str_tolower(buffer);                 /* 转小写后统计 */
        ht_insert(ht, buffer);               /* 插入哈希表 */
        total_words++;
    }
    fclose(fp);

    /* ----- 4. 排序 -----
     * 用到：hash_table.h 的 ht_to_array（哈希表→数组）、cmp_by_freq（比较函数）
     *       <stdlib.h> qsort（标准库快速排序）
     */
    int unique_count;
    WordPair *sorted = ht_to_array(ht, &unique_count);
    qsort(sorted, unique_count, sizeof(WordPair), cmp_by_freq);

    /* ----- 5. 输出 Top 5 和总单词数 -----
     * 用到：<stdio.h> printf
     */
    int top_n = (5 < unique_count) ? 5 : unique_count;
    printf("Top %d most frequent words:\n", top_n);
    for (int i = 0; i < top_n; i++)
        printf("'%s': %d\n", sorted[i].word, sorted[i].count);
    printf("Total words: %d\n", total_words);

    /* ----- 6. 释放内存 -----
     * 用到：hash_table.h 的 ht_destroy（释放哈希表）
     *       <stdlib.h> free（释放排序数组）
     * 顺序：先释放排序数组（只存指针），再释放哈希表（释放实际字符串）
     */
    free(sorted);
    ht_destroy(ht);
    return 0;
}
