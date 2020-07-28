//
// Created by BlackHole on 2020/7/28.
//
#include <stdlib.h>

struct TreeNode {
    int val;
    struct TreeNode *left;
    struct TreeNode *right;
};

struct QueNode {
    struct TreeNode *p;
    struct QueNode *next;
};

void init(struct QueNode **p, struct TreeNode *t) {
    (*p) = (struct QueNode *) malloc(sizeof(struct QueNode));
    (*p)->p = t;
    (*p)->next = NULL;
}

int maxDepth(struct TreeNode *root) {
    if (root == NULL) return 0;
    struct QueNode *left, *right;
    init(&left, root);
    right = left;
    int ans = 0, sz = 1, tmp = 0;
    while (left != NULL) {
        tmp = 0;
        while (sz > 0) {
            if (left->p->left != NULL) {
                init(&right->next, left->p->left);
                right = right->next;
                tmp++;
            }
            if (left->p->right != NULL) {
                init(&right->next, left->p->right);
                right = right->next;
                tmp++;
            }
            left = left->next;
            sz--;
        }
        sz += tmp;
        ans++;
    }
    return ans;
}

int main() {
    struct TreeNode root = {3, NULL, NULL};
    struct TreeNode t1 = {9, NULL, NULL};
    struct TreeNode t2 = {20, NULL, NULL};
    struct TreeNode t3 = {15, NULL, NULL};
    struct TreeNode t4 = {7, NULL, NULL};
    struct TreeNode t5 = {9, NULL, NULL};
    root.left = &t1;
    root.right = &t2;
    root.right->left = &t3;
    root.right->right = &t4;
    root.right->right->right = &t5;
    int i = maxDepth(&root);
    printf("%d", i);
}