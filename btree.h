#ifndef BTREE_H
#define BTREE_H

template <typename K, typename V, int T = 3>
class BTree {
    struct Node {
        bool leaf;
        int n;
        K keys[2 * T - 1];
        V values[2 * T - 1];
        Node* children[2 * T];

        Node(bool isLeaf) {
            leaf = isLeaf;
            n = 0;
            for (int i = 0; i < 2 * T; i++) children[i] = nullptr;
        }
    };

    Node* root;

public:
    BTree() {
        root = new Node(true);
    }

    ~BTree() {
        destroy(root);
    }

    V* search(const K& key) {
        return search(root, key);
    }

    void insert(const K& key, const V& value) {
        if (root->n == 2 * T - 1) {
            Node* s = new Node(false);
            s->children[0] = root;
            splitChild(s, 0, root);
            root = s;
        }
        insertNonFull(root, key, value);
    }

private:
    void destroy(Node* x) {
        if (!x) return;
        if (!x->leaf) {
            for (int i = 0; i <= x->n; i++) {
                destroy(x->children[i]);
            }
        }
        delete x;
    }

    V* search(Node* x, const K& key) {
        int i = 0;
        while (i < x->n && key > x->keys[i]) i++;
        if (i < x->n && key == x->keys[i]) return &x->values[i];
        if (x->leaf) return nullptr;
        return search(x->children[i], key);
    }

    void splitChild(Node* x, int i, Node* y) {
        Node* z = new Node(y->leaf);
        z->n = T - 1;
        for (int j = 0; j < T - 1; j++) {
            z->keys[j] = y->keys[j + T];
            z->values[j] = y->values[j + T];
        }
        if (!y->leaf) {
            for (int j = 0; j < T; j++) {
                z->children[j] = y->children[j + T];
            }
        }
        y->n = T - 1;
        for (int j = x->n; j >= i + 1; j--) {
            x->children[j + 1] = x->children[j];
        }
        x->children[i + 1] = z;
        for (int j = x->n - 1; j >= i; j--) {
            x->keys[j + 1] = x->keys[j];
            x->values[j + 1] = x->values[j];
        }
        x->keys[i] = y->keys[T - 1];
        x->values[i] = y->values[T - 1];
        x->n++;
    }

    void insertNonFull(Node* x, const K& key, const V& value) {
        int i = x->n - 1;
        if (x->leaf) {
            while (i >= 0 && key < x->keys[i]) {
                x->keys[i + 1] = x->keys[i];
                x->values[i + 1] = x->values[i];
                i--;
            }
            x->keys[i + 1] = key;
            x->values[i + 1] = value;
            x->n++;
        } else {
            while (i >= 0 && key < x->keys[i]) i--;
            i++;
            if (x->children[i]->n == 2 * T - 1) {
                splitChild(x, i, x->children[i]);
                if (key > x->keys[i]) i++;
            }
            insertNonFull(x->children[i], key, value);
        }
    }
};

#endif
