#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#define HASH_SIZE 10007  /* 质数桶数，减少哈希冲突 */

/* ---------- 数据结构 ---------- */

/* 哈希桶节点：链表法解决冲突 */
typedef struct WordEntry {
    char *word;              /* 单词字符串（动态分配） */
    int count;               /* 出现次数 */
    struct WordEntry *next;  /* 链表指针：指向同一桶的下一个节点 */
} WordEntry;

/* 哈希表：桶数组 + 唯一单词计数 */
typedef struct {
    WordEntry **buckets;     /* 桶数组（指针数组，每个元素是一条链表头） */
    int unique_count;        /* 不同单词总数 */
} HashTable;

/* 排序用键值对：将哈希表内容转为此结构以便 qsort */
typedef struct {
    char *word;              /* 指向哈希表中单词字符串（不复制，只引用） */
    int count;
} WordPair;

/* ---------- 函数声明 ---------- */

/* 创建哈希表：分配桶数组并初始化为 NULL */
HashTable *ht_create(void);

/* 插入单词：已存在则计数+1，不存在则新建节点（头插法） */
void ht_insert(HashTable *ht, const char *word);

/* 将哈希表所有键值对导出为 WordPair 数组（需 free） */
WordPair *ht_to_array(HashTable *ht, int *out_count);

/* qsort 比较函数：按出现次数降序排列 */
int cmp_by_freq(const void *a, const void *b);

/* 销毁哈希表：释放所有节点、单词字符串、桶数组 */
void ht_destroy(HashTable *ht);

#endif
