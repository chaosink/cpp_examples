#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include <memory>
using namespace std;

class Trie { // unique_ptr
	struct TrieNode {
		constexpr static size_t max_size = 'z' - 'a' + 1;
		unique_ptr<TrieNode> next[max_size];
		bool is_word;
		TrieNode(bool is_word = false): is_word(is_word) {}
	};

	TrieNode root_;

	TrieNode* Find(const string &s) {
		TrieNode *p = &root_;
		for(size_t i = 0; i < s.size() && p != nullptr; ++i)
			p = p->next[s[i] - 'a'].get();
		return p;
	}

public:
	Trie() {}

	// Inserts a word into the trie.
	void Insert(const string &s) {
		TrieNode *p = &root_;
		for(size_t i = 0; i < s.size(); ++i) {
			size_t k = s[i] - 'a';
			if(p->next[k] == nullptr)
				p->next[k] = make_unique<TrieNode>();
			p = p->next[k].get();
		}
		p->is_word = true;
	}

	// Returns if the word is in the trie.
	bool Search(const string &s) {
		TrieNode *p = Find(s);
		return p != nullptr && p->is_word;
	}

	// Returns if there is any word in the trie that starts with the given prefix.
	bool StartsWith(const string &s) {
		return Find(s) != nullptr;
	}
};

/*
class Trie { // manual memory management
	struct TrieNode {
		constexpr static size_t max_size = 'z' - 'a' + 1;
		TrieNode *next[max_size]{};
		bool is_word;
		TrieNode(bool is_word = false): is_word(is_word) {}
	};

	vector<TrieNode*> nodes_;
	TrieNode root_;

	TrieNode* Find(const string &s) {
		TrieNode *p = &root_;
		for(size_t i = 0; i < s.size() && p != nullptr; ++i)
			p = p->next[s[i] - 'a'];
		return p;
	}

public:
	Trie() {}
	~Trie() {
		for(auto node: nodes_) delete node;
	}

	// Inserts a word into the trie.
	void Insert(const string &s) {
		TrieNode *p = &root_;
		for(size_t i = 0; i < s.size(); ++i) {
			size_t k = s[i] - 'a';
			if(p->next[k] == nullptr) {
				nodes_.push_back(new TrieNode);
				p->next[k] = nodes_.back();
			}
			p = p->next[k];
		}
		p->is_word = true;
	}

	// Returns if the word is in the trie.
	bool Search(const string &s) {
		TrieNode *p = Find(s);
		return p != nullptr && p->is_word;
	}

	// Returns if there is any word in the trie that starts with the given prefix.
	bool StartsWith(const string &s) {
		return Find(s) != nullptr;
	}
};
*/

int main() {
	Trie* trie = new Trie;

	trie->Insert("abcd");

	assert(trie->Search("abcd") == true);
	assert(trie->Search("abc") == false);
	assert(trie->Search("abcde") == false);
	assert(trie->Search("xyz") == false);

	assert(trie->StartsWith("abcd") == true);
	assert(trie->StartsWith("abc") == true);
	assert(trie->StartsWith("ab") == true);
	assert(trie->StartsWith("a") == true);

	assert(trie->StartsWith("bcd") == false);
	assert(trie->StartsWith("xyz") == false);
}
