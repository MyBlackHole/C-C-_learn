//
// Created by BlackHole on 2020/8/2.
//

#include "stdio.h"
#include "stdlib.h"

struct TreeNode {
    int value;
    struct TreeNode *left;
    struct TreeNode *right;
};

struct TreeNode *create(const int value) {
    struct TreeNode *node = malloc(sizeof(struct TreeNode));
    if (node == NULL) {
        return NULL;
    }
    node->value = value;
    node->left = NULL;
    node->right = NULL;
    return node;
}

//void insert(struct TreeNode *root, const int value) {
//    while (1) {
//        if (root->left == NULL) {
//            root->left = create(value);
//            return;
//        }
//        if (root->right == NULL) {
//            root->right = create(value);
//            return;
//        }
//        insert(root->left, value);
//        insert(root->right, value);
//    }
//}
int num;

void preorderTraversal(struct TreeNode *root, struct TreeNode ***l);

void flatten(struct TreeNode *root) {
    num = 0;
    struct TreeNode **l = (struct TreeNode **) malloc(0);
    preorderTraversal(root, &l);
    for (int i = 1; i < num; i++) {
        struct TreeNode *prev = l[i - 1], *curr = l[i];
        prev->left = NULL;
        prev->right = curr;
    }
    free(l);
}

void preorderTraversal(struct TreeNode *root, struct TreeNode ***l) {
    if (root != NULL) {
        num++;
        (*l) = (struct TreeNode **) realloc((*l), sizeof(struct TreeNode *) * num);
        (*l)[num - 1] = root;
        preorderTraversal(root->left, l);
        preorderTraversal(root->right, l);
    }
}


int main() {
    struct TreeNode root = {0, NULL, NULL};
    struct TreeNode *node1 = create(10);
    root.left = node1;
    struct TreeNode *node2 = create(20);
    root.right = node2;
    struct TreeNode *node3 = create(40);
    node2->right = node3;
    flatten(&root);
    printf("ok");
}
