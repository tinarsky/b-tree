#include<iostream>
#include "b_tree.h"

int main() {
    BTree<int, std::string> b_tree(3);

    b_tree.insert(1, "a");
    b_tree.insert(3, "b");
    b_tree.insert(7, "c");
    b_tree.insert(10, "d");
    b_tree.insert(11, "e");
    b_tree.insert(13, "f");
    b_tree.insert(14, "g");
    b_tree.insert(15, "h");
    b_tree.insert(18, "i");
    b_tree.insert(16, "j");
    b_tree.insert(19, "k");
    b_tree.insert(24, "l");
    b_tree.insert(25, "m");
    b_tree.insert(26, "n");
    b_tree.insert(21, "o");
    b_tree.insert(4, "p");
    b_tree.insert(5, "q");
    b_tree.insert(20, "r");
    b_tree.insert(22, "s");
    b_tree.insert(2, "t");
    b_tree.insert(17, "u");
    b_tree.insert(12, "v");
    b_tree.insert(6, "w");

    std::cout << "Traversal of tree\n";
    b_tree.traverse(std::cout);
    std::cout << std::endl;

    // 1 2 3 4 5 6 7 10 11 12 13 14 15 16 17 18 19 20 21 22 24 25 26

    for (const auto &e : b_tree) {
        std::cout << " (" << e.key << ", "
                  << e.value << ") ";
    }
    std::cout << std::endl;

    std::cout << "6 in b_tree: " << (b_tree.search(6)->key == 6) << std::endl;

    b_tree.remove(6);
    std::cout << "Traversal of tree after removing 6\n";
    b_tree.traverse(std::cout);
    std::cout << std::endl;

    std::cout << "6 do not present in b_tree: "
              << (b_tree.search(6) == b_tree.end()) << std::endl;

    b_tree.remove(13);
    std::cout << "Traversal of tree after removing 13\n";
    b_tree.traverse(std::cout);
    std::cout << std::endl;

    b_tree.remove(7);
    std::cout << "Traversal of tree after removing 7\n";
    b_tree.traverse(std::cout);
    std::cout << std::endl;

    b_tree.remove(2);
    std::cout << "Traversal of tree after removing 2\n";
    b_tree.traverse(std::cout);
    std::cout << std::endl;

    std::cout << "16 in b_tree: " << (b_tree.search(16)->key == 16)
              << std::endl;

    b_tree.remove(16);
    std::cout << "Traversal of tree after removing 16\n";
    b_tree.traverse(std::cout);
    std::cout << std::endl;

    std::cout << "16 do not present in b_tree: "
              << (b_tree.search(16) == b_tree.end())
              << std::endl;

    for (auto it = b_tree.rbegin(); it != b_tree.rend(); it++) {
        std::cout << " (" << (*it).key << ", "
                  << (*it).value << ") ";
    }
    std::cout << std::endl;

    return 0;
}
