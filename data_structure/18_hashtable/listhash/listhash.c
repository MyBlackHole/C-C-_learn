#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include"listhash.h"

#ifdef MEMORY_TEST
#include<mcheck.h>
#endif


/**
 * @def hash_tab初始化
 * @param size:桶的个数
 * @param hash_value:hash函数
 * @param key_cmp:key比较
 * @param hash_node_free:释放内存
 * @return hash_tab指针
 */
hash_tab *hash_tab_create(int size, hash_key_func hash_value,
                          keycmp_func key_cmp, hash_node_free_func hash_node_free) {
    hash_tab *h = NULL;
    int i = 0;

    if ((size < 0) || (hash_value == NULL) || (key_cmp == NULL)) {
        return NULL;
    }

    h = (hash_tab *) malloc(sizeof(hash_tab));
    if (h == NULL) {
        return NULL;
    }

    /**
     * 申请size份hash_tab_node
     */
    h->htables = (hash_tab_node **) malloc(size * sizeof(hash_tab_node *));
    if (h->htables == NULL) {
        return NULL;
    }

    h->size = size;
    h->nel = 0;
    h->hash_value = hash_value;
    h->keycmp = key_cmp;
    h->hash_node_free = hash_node_free;

    for (i = 0; i < size; i++) {
        h->htables[i] = NULL;
    }

    return h;
}

/**
 * @def 释放桶内存
 * @param h:hash表
 */
void hash_tab_destory(hash_tab *h) {
    int i = 0;
    hash_tab_node *cur = NULL;
    hash_tab_node *tmp = NULL;

    if (h == NULL) {
        return;
    }

    /**
     * 遍历桶
     */
    for (i = 0; i < h->size; i++) {
        cur = h->htables[i];
        while (cur != NULL) {
            tmp = cur;
            cur = cur->next;
            h->hash_node_free(tmp);
        }
        h->htables[i] = NULL;
    }

    free(h->htables);
    free(h);
    return;
}

/**
 * @def 插入
 * @param h:hash表
 * @param key:key
 * @param data:value
 * @return int
 */
int hash_tab_insert(hash_tab *h, void *key, void *data) {
    unsigned int hvalue = 0;
    int i = 0;
    hash_tab_node *cur = NULL;
    hash_tab_node *prev = NULL;
    hash_tab_node *newnode = NULL;

    if ((h == NULL) || (key == NULL) || (data == NULL)) {
        return 1;
    }

    /*获取hash 数值*/
    hvalue = h->hash_value(h, key);
    cur = h->htables[hvalue];

    /*hash桶中元素是从小到大排列的，找到要插入的位置*/
    while ((cur != NULL) && (h->keycmp(h, key, cur->key) > 0)) {
        prev = cur;
        cur = cur->next;
    }

    /*如果key和当前key比对一致，直接返回，数据已经存在*/
    if ((cur != NULL) && (h->keycmp(h, key, cur->key) == 0)) {
        return 2;
    }

    newnode = (hash_tab_node *) malloc(sizeof(hash_tab_node));
    if (newnode == NULL) {
        return 3;
    }

    newnode->key = key;
    newnode->data = data;
    if (prev == NULL) {
        newnode->next = h->htables[hvalue];
        h->htables[hvalue] = newnode;
    } else {
        newnode->next = prev->next;
        prev->next = newnode;
    }

    h->nel++;
    return 0;
}

/**
 * @def 删除hash数据节点
 * @param h:hash表
 * @param key:key
 * @return hash_tab_node
 */
hash_tab_node *hash_tab_delete(hash_tab *h, void *key) {
    int hvalue = 0;
    int i = 0;
    hash_tab_node *cur = NULL;
    hash_tab_node *prev = NULL;

    if ((h == NULL) || (key == NULL)) {
        return NULL;
    }

    /*获取hash 数值*/
    hvalue = h->hash_value(h, key);
    cur = h->htables[hvalue];
    /*hash桶中元素是从小到大排列的，找到要插入的位置*/
    while ((cur != NULL) && (h->keycmp(h, key, cur->key) >= 0)) {
        if (h->keycmp(h, key, cur->key) == 0) {
            if (prev == NULL) {
                h->htables[hvalue] = cur->next;
            } else {
                prev->next = cur->next;
            }
            return cur;
        }
        prev = cur;
        cur = cur->next;
    }

    return NULL;
}

void *hash_tab_search(hash_tab *h, void *key) {
    int hvalue = 0;
    int i = 0;
    hash_tab_node *cur = NULL;

    if ((h == NULL) || (key == NULL)) {
        return NULL;
    }

    /*获取hash 数值*/
    hvalue = h->hash_value(h, key);
    cur = h->htables[hvalue];
    /*hash桶中元素是从小到大排列的，找到要插入的位置*/
    while ((cur != NULL) && (h->keycmp(h, key, cur->key) >= 0)) {
        if (h->keycmp(h, key, cur->key) == 0) {
            return cur->data;
        }
        cur = cur->next;
    }

    return NULL;
}

void hash_tab_dump(hash_tab *h) {
    int i = 0;
    hash_tab_node *cur = NULL;

    if (h == NULL) {
        return;
    }

    printf("\r\n----开始--size[%d],nel[%d]------------", h->size, h->nel);
    for (i = 0; i < h->size; i++) {
        printf("\r\n htables[%d]:", i);
        cur = h->htables[i];
        while ((cur != NULL)) {
            printf("key[%s],data[%s] ", cur->key, cur->data);
            cur = cur->next;
        }
    }

    printf("\r\n----结束--size[%d],nel[%d]------------", h->size, h->nel);
}


/**
 * @def 对字符进行hash计算
 * @param str:key
 * @return key
 */
unsigned int siample_hash(const char *str) {
    register unsigned int hash = 0;
    register unsigned int seed = 131;

    while (*str) {
        hash = hash * seed + *str++;
    }

    return hash & (0x7FFFFFFF);
}

/**
 * @def 桶选择
 * @param h:hash表
 * @param key:key
 * @return int
 */
int hash_tab_hvalue(hash_tab *h, const void *key) {
    return (siample_hash(key) % h->size);
}

/**
 * 缓存
 */
struct test_node {
    char key[80];
    char data[80];
};

/**
 * @def key比较
 * @param h:hash表
 * @param key1:key
 * @param key2:key
 * @return int
 */
int hash_tab_keycmp(hash_tab *h, const void *key1, const void *key2) {
    return strcmp(key1, key2);
}

void hash_tab_node_free(hash_tab_node *node) {
    struct test_node *ptmp = NULL;

    ptmp = container(node->key, struct test_node, key);

    free(ptmp);
    free(node);
}


int main() {

    int i = 0;
    int res = 0;
    char *pres = NULL;
    //hash节点指针声明
    hash_tab_node *node = NULL;
    struct test_node *p = NULL;
    //hashtab指针声明
    hash_tab *h = NULL;
#ifdef MEMORY_TEST
    setenv("MALLOC_TRACE","1.txt",1);
    mtrace();
#endif

    h = hash_tab_create(5, hash_tab_hvalue, hash_tab_keycmp, hash_tab_node_free);
    assert(h != NULL);
    while (1) {
        p = (struct test_node *) malloc(sizeof(struct test_node));
        assert(p != NULL);
        printf("\r\n 请输入key 和value，当可以等于\"quit\"时退出");
        scanf("%s", p->key);
        scanf("%s", p->data);

        if (strcmp(p->key, "quit") == 0) {
            free(p);
            break;
        }

        res = hash_tab_insert(h, p->key, p->data);
        if (res != 0) {
            free(p);
            printf("\r\n key[%s],data[%s] insert failed %d", p->key, p->data, res);
        } else {
            printf("\r\n key[%s],data[%s] insert success %d", p->key, p->data, res);
        }
    }

    hash_tab_dump(h);

    while (1) {
        p = (struct test_node *) malloc(sizeof(struct test_node));
        assert(p != NULL);
        printf("\r\n 请输入key 查询value的数值，当可以等于\"quit\"时退出");
        scanf("%s", p->key);

        if (strcmp(p->key, "quit") == 0) {
            free(p);
            break;
        }
        pres = hash_tab_search(h, p->key);
        if (pres == NULL) {
            printf("\r\n key[%s] search data failed", p->key);
        } else {
            printf("\r\n key[%s],search data[%s] success", p->key, pres);
        }
        free(p);
    }
    hash_tab_dump(h);
    while (1) {
        p = (struct test_node *) malloc(sizeof(struct test_node));
        /**
         * 断言
         */
        assert(p != NULL);
        printf("\r\n 请输入key 删除节点的数值，当可以等于\"quit\"时退出");
        scanf("%s", p->key);

        if (strcmp(p->key, "quit") == 0) {
            free(p);
            break;
        }
        node = hash_tab_delete(h, p->key);
        if (node == NULL) {
            printf("\r\n key[%s] delete node failed ", p->key);
        } else {
            printf("\r\n key[%s],delete data[%s] success", node->key, node->data);
            h->hash_node_free(node);
        }
        free(p);
        hash_tab_dump(h);
    }

    hash_tab_destory(h);
#ifdef MEMORY_TEST
    muntrace();
#endif
    return 0;

}
