#ifndef HASH_TABLE_H  // 判断 ifndef（如果没有定义）：这是一个预处理指令，防止头文件被多次包含导致重复定义错误
#define HASH_TABLE_H  // 定义一个宏 HASH_TABLE_H，表示这个头文件已经被包含过了

#define HASH_SIZE 10007  /* 宏定义：哈希桶的大小。质数桶数，减少哈希冲突（即不同的单词被分配到同一个桶的概率） */

/* 哈希桶节点：链表法解决冲突 */
typedef struct WordEntry {  // 是哈希表里最基本的存储单位
    char *word;              /* 单词字符串（动态分配） */
    int count;               /* 出现次数 */
    struct WordEntry *next;  /* 链表指针：指向同一桶的下一个节点 */
} WordEntry;

/* 哈希表：桶数组 + 不同单词计数 */
typedef struct {
    WordEntry **buckets;     /* buckets 是二级指针，桶数组（指针数组，每个元素是一条链表头） */
    int unique_count;        /* 不同单词总数 */
} HashTable;
/* buckets是整个大表的地址地址，通过他可以访问桶
 * 然后WordEntry就是一个东西（包含word,count,next）放在这些桶中
 * 如果两个WordEntry放在一个桶里面就可以用next进行联系，不至于冲突*/

/* 排序用键值对：将哈希表内容转为此结构以便 qsort */
typedef struct {
    char *word;              /* 指向哈希表中单词字符串（不复制，只引用） */
    int count;
} WordPair;

/* 函数声明 */
/* 创建哈希表：分配桶数组并初始化为 NULL */
HashTable *ht_create(void);  // 返回一个指向 HashTable 结构体的指针，ht_create 函数名，(void) 表示函数不接受任何参数

/* 插入单词：已存在则计数+1，不存在则新建节点（头插法） */
void ht_insert(HashTable *ht, const char *word);  // HashTable *ht 是目标仓库，const char *word 是要插入的单词字符串（const 保证只读，不修改）

/* 将哈希表所有键值对导出为 WordPair 数组（需 free） */
WordPair *ht_to_array(HashTable *ht, int *out_count);  // out_count 来接收unique_count的地址

/* qsort 的比较函数：按出现次数降序排列 */
int cmp_by_freq(const void *a, const void *b);  // 返回一个整数，表示 a 和 b 的相对顺序，a 应该排在 b 前面（返回负数），还是后面（返回正数），还是相等（返回0）

/* 销毁哈希表：释放所有节点、单词字符串、桶数组 */
void ht_destroy(HashTable *ht);

#endif  // 判断结束
