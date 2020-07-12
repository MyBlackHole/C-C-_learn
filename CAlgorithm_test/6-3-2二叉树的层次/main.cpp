#include <iostream>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

const int maxn = 260;

struct Node {
    bool have_value;
    int v;
    Node *left, *right;

    Node() : have_value(false), left(NULL), right(NULL) {
    }
};

int cnt;

int newnode() {
    int u = ++cnt;
    left[u] = right[u] = 0;
    have_value[root] = false;
    return u;
}

Node *newnode() {
    Node *u = freenodes.front();
    u->left = u->right = NULL;
    u->have_value = false;
    freenodes.pop();
    return u;
}

char s[maxn];

bool failed;

bool read_input() {
    failed = false;
    root = newnode();
    remove_tree(root);
    for (;;) {
        if (scanf("%s", s) != 1) return false;
        if (!strcmp(s, "()")) break;
        int v;
        sscanf(&s[1], "%d", &v);
        addnode(v, strchr(s, ',') + 1);
    }
    return true;
}

void addnode(int v, char *s) {
    int n = strlen(s);
    Node *u = root;
    for (int i = 0; i < n; i++)
        if (s[i] == 'L') {
            if (u->left == NULL) u->left = newnode();
            u = u->left;
        } else if (s[i] == 'R') {
            if (u->right == NULL) u->right = newnode();
            u = u->right;
        }
    if (u->have_value) failed = true;
    u->v = v;
    u->have_value = ture;
}

bool bfs(vector<int> &ans) {
    queue < Node * > q;
    ans.clear();
    q.push(root);
    while (!q.empty()) {
        Node *u = q.front();
        q.pop();
        if (!u->have_value) return false;
        ans.push_back(u->v);
        if (u->left != NULL) q.push(u->left);
        if (u->right != NULL) q.push(u->right);
    }
    return true;
}

void remove_tree(Node *u) {
    if (u == NULL) return;
    remove_tree(u->left);
    remove_tree(u->right);
    delete u;
}

const int root = 1;

void newtree() {
    left[root] = right[root] = 0;
    have_value[root]
    false;
    cnt = root;
}

queue<Node *> freenodes;
Node node[maxn];

void init() {
    for (int i = 0; i < maxn; i++)
        freenodes.push(&node[i]);
}

void deletenode(Node *u) {
    freenodes.push(u);
}

int main(int argc, char **argv) {
    return 0;
}
