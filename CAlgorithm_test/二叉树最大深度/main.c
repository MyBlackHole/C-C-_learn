//
// Created by BlackHole on 2020/7/28.
//

#include <stdio.h>
#include <math.h>

struct TreeNode {
    int val;
    struct TreeNode *left;
    struct TreeNode *right;
};

//int maxDepth(struct TreeNode *root) {
//    int i = 0, j = 0;
//    int z;
//    if (root->left != NULL) {
//        i = maxDepth(root->left);
//    }
//    if (root->right != NULL) {
//        j = maxDepth(root->right);
//    }
//    z = i >= j ? i : j;
//    return z >= 1 ? z + 1 : 1;
//}

int maxDepth(struct TreeNode *root) {
    if (root == NULL) return 0;
    return fmax(maxDepth(root->left), maxDepth(root->right)) + 1;
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
//    if (t1.right == NULL) {
//        printf("ok");
//    }
//    printf("%d", root.left->val);
//    printf("%d", NULL);
}