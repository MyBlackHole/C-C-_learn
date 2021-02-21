#include <iostream>

struct TreeNode;
typedef TreeNode *Node;
typedef int EleType;

struct TreeNode {
    Node lchild;
    Node rchild;
    EleType data;
};


void PreOrderTree(Node node) {
    if (node != NULL) {
        printf("%d\n", node->data);
        PreOrderTree(node->lchild);
        PreOrderTree(node->rchild);
    }
}

void InOrderTree(Node node) {
    if (node != NULL) {
        InOrderTree(node->lchild);
        printf("%d\n", node->data);
        InOrderTree(node->rchild);
    }

}

void AfterPreOrderTree(Node node) {
    if (node != NULL) {
        AfterPreOrderTree(node->lchild);
        AfterPreOrderTree(node->rchild);
        printf("%d\n", node->data);
    }
}

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char **argv) {
    return 0;
}
