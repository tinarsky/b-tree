#include <gtest/gtest.h>

#include <ranges>
#include <utility>
#include "b_tree.h"

TEST(BTreeTests, InsertTest) {
    BTree<int, char> b_tree(3);
    b_tree.insert(3, 'c');
    b_tree.insert(1, 'a');
    b_tree.insert(2, 'b');
    b_tree.insert(5, 'e');
    b_tree.insert(4, 'd');
    b_tree.insert(8, 'h');
    b_tree.insert(7, 'g');
    b_tree.insert(6, 'f');

    EXPECT_EQ(b_tree.size(), 8);

    int i = 1;
    for (auto e : b_tree) {
        EXPECT_EQ(i, e.key);
        EXPECT_EQ('a' + i - 1, e.value);
        i++;
    }
    EXPECT_LE(i, 10);
}

TEST(BTreeTests, SearchTest) {
    BTree<std::string, std::string> b_tree(3);
    b_tree.insert("Ivan", "Ivanov");
    b_tree.insert("Anton", "Andreev");
    b_tree.insert("Kirill", "Petrov");

    EXPECT_EQ(b_tree.search("Ivan"), ++b_tree.begin());
    EXPECT_EQ(b_tree.search("Kirill")->value, "Petrov");
}

TEST(BTreeTests, RemoveTest) {
    BTree<int, bool> b_tree(3);
    b_tree.insert(1, true);
    b_tree.insert(3, true);
    b_tree.insert(7, false);
    b_tree.insert(10, true);
    b_tree.insert(11, false);
    b_tree.insert(13, false);
    b_tree.insert(14, true);
    b_tree.insert(15, false);
    b_tree.insert(18, true);
    b_tree.insert(16, false);
    b_tree.insert(19, true);
    b_tree.insert(24, true);
    b_tree.insert(25, false);
    b_tree.insert(26, true);

    EXPECT_NE(b_tree.search(14), b_tree.end());

    b_tree.remove(14);

    EXPECT_EQ(b_tree.search(14), b_tree.end());
}

TEST(BTreeTests, IteratorTest) {
    BTree<std::string, int> b_tree(4);
    b_tree.insert("aaa", 3);
    b_tree.insert("ba", 9);
    b_tree.insert("bb", 10);
    b_tree.insert("aac", 5);
    b_tree.insert("aab", 4);
    b_tree.insert("ad", 8);
    b_tree.insert("ac", 7);
    b_tree.insert("ab", 6);
    b_tree.insert("bc", 11);
    b_tree.insert("d", 12);

    int i = 3;
    for (auto e : b_tree) {
        EXPECT_EQ(i, e.value);
        i++;
    }
    EXPECT_EQ(i, 13);

    i = 12;
    for (auto it = b_tree.rbegin(); it != b_tree.rend(); it++) {
        EXPECT_EQ(i, (*it).value);
        i--;
    }
    EXPECT_EQ(i, 2);
}

TEST(BTreeTests, FillTest) {
    BTree<std::string, int> b_tree(4);
    b_tree.insert("aaa", 1);
    b_tree.insert("ba", 7);
    b_tree.insert("bb", 8);
    b_tree.insert("aac", 3);
    b_tree.insert("aab", 2);
    b_tree.insert("ad", 6);
    b_tree.insert("ac", 5);
    b_tree.insert("ab", 4);
    b_tree.insert("bc", 9);
    b_tree.insert("d", 10);

    std::fill(b_tree.begin(), --b_tree.end(),
              BTree<std::string, int>::Entry("test", 0));

    auto it = b_tree.begin();
    for (int i = 1; i < 10; i++) {
        EXPECT_EQ("test", it->key);
        EXPECT_EQ(0, it->value);
        it++;
    }

    EXPECT_EQ("d", it->key);
    EXPECT_EQ(10, it->value);
}

TEST(BTreeTests, FindIfTest) {
    struct SomeType {
        std::string s;
        int n;
        SomeType() = default;
        SomeType(std::string s, int n) : s(std::move(s)), n(n) {}
    };

    BTree<int, SomeType> b_tree(4);
    b_tree.insert(1, SomeType("s", 3));
    b_tree.insert(2, SomeType("a", 7));
    b_tree.insert(3, SomeType("t", 23));
    b_tree.insert(4, SomeType("test", 90));
    b_tree.insert(5, SomeType("word", 5));
    b_tree.insert(6, SomeType("b", 8));
    b_tree.insert(7, SomeType("test", 90));

    auto it = std::find_if(b_tree.begin(), --b_tree.end(),
                           [](const BTree<int, SomeType>::Entry &entry) {
                               return entry.value.s == "test";
                           });

    EXPECT_EQ(4, it->key);
    EXPECT_EQ(90, it->value.n);
    EXPECT_EQ("test", it->value.s);
}
