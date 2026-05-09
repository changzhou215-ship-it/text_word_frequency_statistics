#include <stdio.h>   /* fprintf, stderr */
#include <stdlib.h>  /* malloc, free, calloc */
#include <string.h>  /* strlen, strcpy, strcmp */
#include "hash_table.h"  // 导入自定义的哈希表头文件，包含 HashTable 结构体定义和相关函数声明

/* 哈希函数 —— djb2 算法
 * 特点：计算快、分布均匀、冲突少
 * 原理：hash = hash * 33 + 当前字符ASCII值
 */
static unsigned int hash(const char *str) {
    unsigned int h = 5381;  // 初始化哈希值为一个大质数，5381 是 djb2 算法的推荐初始值
    int c;
    while ((c = *str++))
        h = ((h << 5) + h) + c;         /* h * 33 + c，h << 5 位运算加速 ，尽量让不同字符的哈希值h差异明显，防止哈希冲突 */
    
        return h % HASH_SIZE;                /* 取模映射到桶范围(0~10006) 只执行一次*/
}

/* ht_create — 创建哈希表
 * calloc 自动将桶数组清零（所有指针初始为 NULL）
 */
HashTable *ht_create(void) {
    HashTable *ht = (HashTable *)malloc(sizeof(HashTable));
    if (!ht) {  // 检查 malloc 是否成功分配内存
        fprintf(stderr, "Error: malloc failed in ht_create\n");
        exit(1);  // exit(1) 表示程序异常退出，这里用 1 表示内存分配失败
    }
    ht->buckets = (WordEntry **)calloc(HASH_SIZE, sizeof(WordEntry *));  // calloc 把申请的内存初始化为0，所有桶指针初始为 NULL
    if (!ht->buckets) {  // 检查 calloc 是否成功分配内存
        fprintf(stderr, "Error: calloc failed in ht_create\n");
        free(ht);  // 先释放之前分配的哈希表结构体内存，避免内存泄漏
        exit(1);
    }
    ht->unique_count = 0;
    return ht;
}

/* ht_find — 在哈希表中查找单词
 * 返回匹配节点的指针，找不到则返回 NULL
 * 遍历对应桶的链表，逐个比较字符串
 */
static WordEntry *ht_find(HashTable *ht, const char *word) {
    unsigned int idx = hash(word);  // index 是通过哈希函数计算得到的桶索引，指向对应的链表头
    WordEntry *entry = ht->buckets[idx];
    while (entry) {  // 遍历链表，直到找到匹配的单词或链表末尾
        if (strcmp(entry->word, word) == 0)
            return entry;                /* 找到：返回节点指针 */
        entry = entry->next;             /* 没找到：继续遍历链表 */
    }
    return NULL;
}

/* ht_insert — 插入单词（若已存在则计数+1）
 * 头插法：新节点插入链表头部，O(1) 无需遍历到尾
 */
void ht_insert(HashTable *ht, const char *word) {
    /* 先查找是否已存在 */
    WordEntry *found = ht_find(ht, word);
    if (found) {
        found->count++;                  /* 已存在：计数+1即可 */
        return;
    }

    /* 新单词：分配节点 + 复制字符串 */
    WordEntry *new_entry = (WordEntry *)malloc(sizeof(WordEntry));
    size_t len = strlen(word);
    new_entry->word = (char *)malloc(len + 1);  /* +1 给 '\0' */
    strcpy(new_entry->word, word);
    new_entry->count = 1;

    /* 头插法：新节点指向原链表头，桶指针指向新节点 */
    unsigned int idx = hash(word);
    new_entry->next = ht->buckets[idx];
    ht->buckets[idx] = new_entry;
    ht->unique_count++;
}

/*
 * ht_to_array — 将哈希表导出为 WordPair 数组
 * 遍历所有桶的所有节点，收集到连续数组中供 qsort 排序
 * 注意：word 字段指向哈希表内部字符串（不复制），
 *       数组使用期间不要释放哈希表
 */
WordPair *ht_to_array(HashTable *ht, int *out_count) {
    *out_count = ht->unique_count;
    WordPair *arr = (WordPair *)malloc(ht->unique_count * sizeof(WordPair));
    int idx = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        WordEntry *entry = ht->buckets[i];
        while (entry) {
            arr[idx].word = entry->word;      /* 只存指针，不复制 */
            arr[idx].count = entry->count;
            idx++;
            entry = entry->next;
        }
    }
    return arr;
}

/*
 * cmp_by_freq — qsort 比较函数
 * 按出现次数降序：b.count - a.count（返回值 > 0 则 b 排前面）
 */
int cmp_by_freq(const void *a, const void *b) {
    const WordPair *pa = (const WordPair *)a;
    const WordPair *pb = (const WordPair *)b;
    return pb->count - pa->count;
}

/*
 * ht_destroy — 销毁哈希表，释放所有内存
 * 遍历每个桶的链表，逐节点释放 word 字符串和节点本身
 */
void ht_destroy(HashTable *ht) {
    for (int i = 0; i < HASH_SIZE; i++) {
        WordEntry *entry = ht->buckets[i];
        while (entry) {
            WordEntry *next = entry->next;
            free(entry->word);               /* 先释放字符串 */
            free(entry);                     /* 再释放节点 */
            entry = next;
        }
    }
    free(ht->buckets);                       /* 释放桶数组 */
    free(ht);                                /* 释放哈希表本身 */
}
