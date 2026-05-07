#include <stdio.h>   /* fprintf, stderr */
#include <stdlib.h>  /* malloc, free, calloc */
#include <string.h>  /* strlen, strcpy, strcmp */
#include "hash_table.h"

/* ================================================================
 * 哈希函数 —— djb2 算法 (Dan Bernstein)
 * 特点：计算快、分布均匀、冲突少
 * 原理：hash = hash * 33 + 当前字符ASCII值
 * ================================================================ */
static unsigned int hash(const char *str) {
    unsigned int h = 5381;               /* 初始魔术数 */
    int c;
    while ((c = *str++))
        h = ((h << 5) + h) + c;         /* h * 33 + c，位运算加速 */
    return h % HASH_SIZE;                /* 取模映射到桶范围 */
}

/* ---------------------------------------------------------------
 * ht_create — 创建哈希表
 * calloc 自动将桶数组清零（所有指针初始为 NULL）
 * ---------------------------------------------------------------*/
HashTable *ht_create(void) {
    HashTable *ht = (HashTable *)malloc(sizeof(HashTable));
    if (!ht) {
        fprintf(stderr, "Error: malloc failed in ht_create\n");
        exit(1);
    }
    ht->buckets = (WordEntry **)calloc(HASH_SIZE, sizeof(WordEntry *));
    if (!ht->buckets) {
        fprintf(stderr, "Error: calloc failed in ht_create\n");
        free(ht);
        exit(1);
    }
    ht->unique_count = 0;
    return ht;
}

/* ---------------------------------------------------------------
 * ht_find — 在哈希表中查找单词
 * 返回匹配节点的指针，找不到则返回 NULL
 * 遍历对应桶的链表，逐个比较字符串
 * ---------------------------------------------------------------*/
static WordEntry *ht_find(HashTable *ht, const char *word) {
    unsigned int idx = hash(word);
    WordEntry *entry = ht->buckets[idx];
    while (entry) {
        if (strcmp(entry->word, word) == 0)
            return entry;                /* 找到：返回节点指针 */
        entry = entry->next;             /* 没找到：继续遍历链表 */
    }
    return NULL;
}

/* ---------------------------------------------------------------
 * ht_insert — 插入单词（若已存在则计数+1）
 * 头插法：新节点插入链表头部，O(1) 无需遍历到尾
 * ---------------------------------------------------------------*/
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

/* ---------------------------------------------------------------
 * ht_to_array — 将哈希表导出为 WordPair 数组
 * 遍历所有桶的所有节点，收集到连续数组中供 qsort 排序
 * 注意：word 字段指向哈希表内部字符串（不复制），
 *       数组使用期间不要释放哈希表
 * ---------------------------------------------------------------*/
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

/* ---------------------------------------------------------------
 * cmp_by_freq — qsort 比较函数
 * 按出现次数降序：b.count - a.count（返回值 > 0 则 b 排前面）
 * ---------------------------------------------------------------*/
int cmp_by_freq(const void *a, const void *b) {
    const WordPair *pa = (const WordPair *)a;
    const WordPair *pb = (const WordPair *)b;
    return pb->count - pa->count;
}

/* ---------------------------------------------------------------
 * ht_destroy — 销毁哈希表，释放所有内存
 * 遍历每个桶的链表，逐节点释放 word 字符串和节点本身
 * ---------------------------------------------------------------*/
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
