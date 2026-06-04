/* hash_table.c - 哈希表实现文件（djb2 + 链地址法）
 * 这个文件实现了哈希表的基本功能，包括创建、插入、查找、导出和销毁。哈希表使用链地址法处理冲突，桶数组大小为 10007（一个质数），以保证较好的性能和较低的冲突率。
 */

/* 依赖：hash_table.h（结构体定义和函数声明）
 * <stdio.h>  — fprintf, stderr
 * <stdlib.h> — malloc, free, calloc, exit, qsort
 * <string.h> — strlen, strcpy, strcmp
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_table.h"

/* hash — djb2 哈希函数
 * 功能：将字符串映射到 [0, HASH_SIZE-1] 的桶索引
 * 参数 str：要哈希的字符串（只读，不修改）
 * 返回：桶索引（范围 0 ~ HASH_SIZE-1）
 * 原理：h = ((h << 5) + h) + c  即 h = h * 33 + c
 * 5381 是 djb2 算法的推荐初始种子
 * h << 5 是乘以 32 的位运算，+ h 得到乘以 33
 */
static unsigned int hash(const char *str) {
    unsigned int h = 5381;      /* djb2 推荐的初始种子值（质数） */
    int c;                      /* 当前字符的 ASCII 值 */
    while ((c = *str++))        /* 逐字符遍历，遇到 '\0' 结束 */
        h = ((h << 5) + h) + c; /* h = h * 33 + c，位运算加速乘法 */
    return h % HASH_SIZE;       /* 取模映射到桶范围 [0, HASH_SIZE-1] */
}

/* ht_create — 创建哈希表
 * 功能：在堆上分配 HashTable 结构体，初始化桶数组，所有桶指针初始化为 NULL（空链表）
 * 返回：指向新创建的 HashTable 的指针
 * 退出条件：若 malloc 或 calloc 失败，打印错误并 exit(1)
 * 注意：应在使用完毕后调用 ht_destroy() 释放内存
 */
HashTable *ht_create(void) {
    /* 步骤 1：为 HashTable 结构体本身分配内存 */
    HashTable *ht = (HashTable *)malloc(sizeof(HashTable));
    if (!ht) {
        fprintf(stderr, "Error: malloc failed in ht_create\n");
        exit(1);
    }

    /* 步骤 2：为桶数组分配内存（calloc 自动清零，所有指针初始为 NULL） */
    ht->buckets = (WordEntry **)calloc(HASH_SIZE, sizeof(WordEntry *));
    if (!ht->buckets) {
        fprintf(stderr, "Error: calloc failed in ht_create\n");
        free(ht);           /* 先释放之前分配的 ht，避免内存泄漏 */
        exit(1);
    }

    /* 步骤 3：初始化唯一单词计数为 0 */
    ht->unique_count = 0;
    return ht;
}


/* ht_find — 在哈希表中查找单词（内部函数，static）
 * 功能：在哈希表中查找指定单词，返回匹配节点的指针，遍历对应桶的链表，用 strcmp 逐节点比较
 * 参数 ht ：目标哈希表
 * 参数 word：要查找的单词
 * 返回：找到则返回 WordEntry 节点指针，否则返回 NULL
 * 注意：这是一个 static 函数，仅在当前文件内部使用
 */
static WordEntry *ht_find(HashTable *ht, const char *word) {
    /* 计算桶索引，定位到对应的链表头 */
    unsigned int idx = hash(word);
    WordEntry *entry = ht->buckets[idx];

    /* 遍历链表，逐个比较单词字符串 */
    while (entry) {
        if (strcmp(entry->word, word) == 0)
            return entry;           /* 找到匹配：返回节点指针 */
        entry = entry->next;        /* 不匹配：继续遍历下一个节点 */
    }
    return NULL;                    /* 遍历完整个链表都没找到 */
}


/* ht_insert — 插入单词
 * 功能：向哈希表中插入一个单词：若单词已存在，将其计数 +1；若单词不存在，创建新节点并采用头插法加入链表
 * 参数 ht ：目标哈希表
 * 参数 word：要插入的单词
 */
void ht_insert(HashTable *ht, const char *word) {
    /* 第一步：先查找单词是否已存在 */
    WordEntry *found = ht_find(ht, word);
    if (found) {
        found->count++;             /* 已存在：仅计数 +1，无需重复存储 */
        return;
    }

    /* 第二步：新单词，分配节点和字符串内存 */
    WordEntry *new_entry = (WordEntry *)malloc(sizeof(WordEntry));
    size_t len = strlen(word);                      /* strlen 返回长度，不含 '\0' */
    new_entry->word = (char *)malloc(len + 1);      /* +1 给结尾的 '\0' 字符 */
    strcpy(new_entry->word, word);                  /* 复制字符串到新分配的空间 */
    new_entry->count = 1;                           /* 新单词计数初始化为 1 */

    /* 第三步：头插法 — 新节点插入链表头部
     * 1. 计算桶索引
     * 2. 新节点的 next 指向当前链表头
     * 3. 桶指针指向新节点（新节点成为新的链表头）
     */
    unsigned int idx = hash(word);
    new_entry->next = ht->buckets[idx];     /* 新节点指向旧链表头 */
    ht->buckets[idx] = new_entry;           /* 桶指针指向新节点 */
    ht->unique_count++;                     /* 唯一单词总数 +1 */
}


/* ht_to_array — 哈希表 → WordPair 数组导出
 * 功能：遍历所有桶的所有节点，将键值对收集到连续数组中，供外部 qsort 排序使用
 * 参数 ht ：源哈希表
 * 参数 out_count：输出参数，接收唯一单词总数
 * 返回：动态分配的 WordPair 数组（调用者需 free）
 * 关键设计：word 字段只存储指向哈希表内部字符串的指针，不复制字符串本身，以节省内存。因此在使用数组期间不得释放哈希表，否则会指针悬空。
 */
WordPair *ht_to_array(HashTable *ht, int *out_count) {
    *out_count = ht->unique_count;                          /* 传出唯一单词数 */
    WordPair *arr = (WordPair *)malloc(                     /* 分配 WordPair 数组 */
        ht->unique_count * sizeof(WordPair)
    );

    int idx = 0;
    /* 外层循环：遍历 10007 个桶 */
    for (int i = 0; i < HASH_SIZE; i++) {
        WordEntry *entry = ht->buckets[i];
        /* 内层循环：遍历当前桶的整条链表 */
        while (entry) {
            arr[idx].word = entry->word;    /* 只存指针，不复制字符串 */
            arr[idx].count = entry->count;  /* 复制计数值 */
            idx++;
            entry = entry->next;            /* 移动到下一个节点 */
        }
    }
    return arr;                             /* 返回动态分配的数组首地址 */
}

/* cmp_by_freq — qsort 比较函数（频率降序）
 * 功能：供 qsort 使用的比较回调，按出现次数从高到低排序，返回值语义：正数 → pb 排在前面（降序）
 * 参数 a, b：指向两个 WordPair 的 const void 指针
 * 返回 ：正数（a < b）、0（a == b）、负数（a > b）
 * 注意：qsort 要求比较函数参数类型为 const void *
 */
int cmp_by_freq(const void *a, const void *b) {
    /* 将 void 指针转型为 WordPair 指针以便访问字段 */
    const WordPair *pa = (const WordPair *)a;
    const WordPair *pb = (const WordPair *)b;
    /* pb->count - pa->count：若 pb 更大则返回正数，pb 排在前面 */
    return pb->count - pa->count;
}


/* ht_destroy — 销毁哈希表，释放所有内存
 * 功能：释放哈希表占用的全部动态内存， 遵循"先分配的后释放，后分配的先释放"原则
 * 释放顺序（由内到外，与分配顺序相反）：
 * 1. 每个节点的 word 字符串，最内层分配
 * 2. 每个 WordEntry 节点
 * 3. 桶数组（buckets）
 * 4. HashTable 结构体本身，最外层分配
 * 注意：使用 ht_to_array 导出的数组必须在调用此函数前释放，销毁后 ht 指针成为悬空指针，调用者不应再使用
 */
void ht_destroy(HashTable *ht) {
    /* 遍历所有桶 */
    for (int i = 0; i < HASH_SIZE; i++) {
        WordEntry *entry = ht->buckets[i];
        /* 遍历当前桶的链表，逐个释放节点 */
        while (entry) {
            WordEntry *next = entry->next;  /* 先保存下一个节点的指针 */
            free(entry->word);              /* 第 1 步：释放 word 字符串 */
            free(entry);                    /* 第 2 步：释放节点本身 */
            entry = next;                   /* 移动到下一个节点 */
        }
    }
    free(ht->buckets);  /* 第 3 步：释放桶数组 */
    free(ht);           /* 第 4 步：释放哈希表结构体 */
}
