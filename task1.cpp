// Task 1: Binary Tree Representation and Operations
// Tree format: label(left,right), a child can be empty e.g. c(,f)

#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <cctype>
using namespace std;

// cast to unsigned char so isalnum is safe for non-ASCII chars
bool isLabelChar(char c) {
    return isalnum((unsigned char)c);
}


// Part 5: validate string without building the tree. Checks that each '(' has
// one ',' and a matching ')', brackets follow a label, no x(,) and no
// leftover characters. Time: O(n), Space: O(h)
bool checkNode(const string& s, size_t& i, bool& isEmpty) {
    isEmpty = true;
    if (i >= s.size() || !isLabelChar(s[i]))
        return true;    // empty child, caller decides if allowed

    isEmpty = false;
    while (i < s.size() && isLabelChar(s[i]))
        i++;

    if (i < s.size() && s[i] == '(') {
        i++;
        bool leftEmpty, rightEmpty;

        if (!checkNode(s, i, leftEmpty))
            return false;
        if (i >= s.size() || s[i] != ',')
            return false;
        i++;

        if (!checkNode(s, i, rightEmpty))
            return false;
        if (i >= s.size() || s[i] != ')')
            return false;
        i++;

        if (leftEmpty && rightEmpty)
            return false;
    }
    return true;
}

bool isValidTreeString(const string& s) {
    size_t i = 0;
    bool isEmpty;
    if (!checkNode(s, i, isEmpty))
        return false;
    return !isEmpty && i == s.size();   // need a root and whole string used
}


struct Node {
    string data;
    Node* left;
    Node* right;

    Node(const string& value) {
        data = value;
        left = nullptr;
        right = nullptr;
    }
};

class BinaryTree {
private:
    Node* root;

    // parses one node from position i, returns nullptr for an empty child
    // (string is validated before this is called)
    Node* parse(const string& s, size_t& i) {
        if (i >= s.size() || !isLabelChar(s[i]))
            return nullptr;

        string label;   // label may be more than one char
        while (i < s.size() && isLabelChar(s[i])) {
            label += s[i];
            i++;
        }
        Node* node = new Node(label);

        // '(' means this node has children
        if (i < s.size() && s[i] == '(') {
            i++;
            node->left = parse(s, i);
            i++;    // skip ','
            node->right = parse(s, i);
            i++;    // skip ')'
        }
        return node;
    }

    void preorder(Node* node) {
        if (node == nullptr)
            return;
        cout << node->data << " ";
        preorder(node->left);
        preorder(node->right);
    }

    // rebuilds the bracket string (this is a preorder walk)
    string toBracketString(Node* node) {
        if (node == nullptr)
            return "";
        string result = node->data;
        if (node->left != nullptr || node->right != nullptr)
            result += "(" + toBracketString(node->left) + "," + toBracketString(node->right) + ")";
        return result;
    }

    // counts edges: empty tree = -1, single node = 0
    int height(Node* node) {
        if (node == nullptr)
            return -1;
        return 1 + max(height(node->left), height(node->right));
    }

    void printDepths(Node* node, int depth) {
        if (node == nullptr)
            return;
        cout << "   " << node->data << " : depth " << depth << endl;
        printDepths(node->left, depth + 1);
        printDepths(node->right, depth + 1);
    }

    void destroy(Node* node) {
        if (node == nullptr)
            return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    BinaryTree() {
        root = nullptr;
    }

    ~BinaryTree() {
        destroy(root);
    }

    // Part 1: build tree from string, refuses invalid strings
    // Time: O(n), Space: O(h)
    bool buildFromString(const string& s) {
        if (!isValidTreeString(s))
            return false;
        destroy(root);
        size_t i = 0;
        root = parse(s, i);
        return true;
    }

    // Part 2: recursive preorder - Time: O(n), Space: O(h)
    void printPreorder() {
        preorder(root);
        cout << endl;
    }

    string getBracketString() {
        return toBracketString(root);
    }

    // Part 3: inorder using a stack: go left pushing nodes, pop and print, go right
    // Time: O(n), Space: O(h)
    void printInorderIterative() {
        stack<Node*> st;
        Node* current = root;

        while (current != nullptr || !st.empty()) {
            while (current != nullptr) {
                st.push(current);
                current = current->left;
            }
            current = st.top();
            st.pop();
            cout << current->data << " ";
            current = current->right;
        }
        cout << endl;
    }

    // Part 4: postorder using one stack: go left pushing nodes; a node is printed
    // only once its right subtree is done (no right child, or we just came
    // back from it, tracked by lastVisited). Time: O(n), Space: O(h)
    void printPostorderIterative() {
        stack<Node*> st;
        Node* current = root;
        Node* lastVisited = nullptr;

        while (current != nullptr || !st.empty()) {
            if (current != nullptr) {
                st.push(current);
                current = current->left;
            }
            else {
                Node* top = st.top();
                if (top->right != nullptr && lastVisited != top->right) {
                    current = top->right;   // right subtree not done yet
                }
                else {
                    cout << top->data << " ";
                    lastVisited = top;
                    st.pop();
                }
            }
        }
        cout << endl;
    }

    // Part 6: height and node depths - Time: O(n) each, Space: O(h)
    int getHeight() {
        return height(root);
    }

    void printAllDepths() {
        printDepths(root, 0);
    }
};


int main() {
    string input = "a(b(d,e),c(,f(g,)))";
    cout << "Given string: " << input << endl << endl;

    BinaryTree tree;

    cout << "Part 1: Build tree from string" << endl;
    if (!tree.buildFromString(input)) {
        cout << "   Given string is invalid" << endl;
        return 1;
    }
    cout << "   Tree built successfully" << endl;

    // a bad string gets rejected instead of building a broken tree
    string bad = "a(b,c";
    if (!tree.buildFromString(bad))
        cout << "   \"" << bad << "\" rejected, not a valid tree" << endl;
    cout << endl;

    cout << "Part 2: Preorder (recursive)" << endl;
    cout << "   Nodes:  ";
    tree.printPreorder();
    cout << "   String: " << tree.getBracketString() << endl << endl;

    cout << "Part 3: Inorder (using stack)" << endl;
    cout << "   ";
    tree.printInorderIterative();
    cout << endl;

    cout << "Part 4: Postorder (iterative)" << endl;
    cout << "   ";
    tree.printPostorderIterative();
    cout << endl;

    cout << "Part 5: Validating strings (no tree built)" << endl;
    vector<string> tests = {
        input,
        "a(b,c)",
        "a",
        "a(b(d,e),c(,f(g,))",   // missing ')'
        "a(b,c,d)",              // three children
        "a(,)",                  // both children empty
        "(a,b)",                 // no root
        "a(b)c"                  // no comma, extra text
    };
    for (int i = 0; i < (int)tests.size(); i++) {
        cout << "   " << tests[i] << "  ->  ";
        cout << (isValidTreeString(tests[i]) ? "Valid" : "Invalid") << endl;
    }
    cout << endl;

    cout << "Part 6: Height and depth" << endl;
    cout << "   Height of tree: " << tree.getHeight() << endl;
    tree.printAllDepths();

    return 0;
}
