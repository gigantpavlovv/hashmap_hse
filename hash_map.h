#include <iostream>
#include <list>
#include <vector>
#include <stdexcept>
using namespace std;

template<typename KeyType, typename ValueType, typename Hash = std::hash<KeyType> >
class HashMap{
    size_t ALPHA = 2;
    size_t MIN_SIZE = 1;
    Hash hasher = Hash();
    list<pair<const KeyType, ValueType>> elems;
    vector<pair<typename list<pair<const KeyType, ValueType>>::iterator, typename list<pair<const KeyType, ValueType>>::iterator>> table;
    size_t count = 0;
    void rebuild() {
        count = 0;
        list<pair<KeyType, ValueType>> save;
        for (auto elem : elems) {
            save.insert(save.begin(), elem);
        }
        elems.clear();
        for (size_t i = 0; i < table.size(); i++) {
            table[i].first = table[i].second = elems.end();
        }
        for (auto elem : save) {
            insert(elem);
        }
    }
public:
    class iterator{
        typename list<pair<const KeyType, ValueType>>::iterator it;
    public:
        iterator() {
        }
        iterator(typename list<pair<const KeyType, ValueType>>::iterator it) : it(it) {
        }
        iterator& operator++() {
            it++;
            return *this;
        }
        iterator& operator--() {
            it--;
            return *this;
        }
        iterator operator++(int) {
            iterator save = *this;
            it++;
            return save;
        }
        iterator operator--(int) {
            iterator save = *this;
            it--;
            return save;
        }
        bool operator==(const iterator& b) {
            return (it == b.it);
        }
        bool operator!=(const iterator& b) {
            return (it != b.it);
        }
        const pair<const KeyType, ValueType>& operator*() const {
            return *it;
        }
        const typename list<pair<const KeyType, ValueType>>::iterator& operator->() const {
            return it;
        }
    };


    class const_iterator{
        typename list<pair<const KeyType, ValueType>>::const_iterator it;
    public:
        const_iterator() {
        }
        const_iterator(typename list<pair<const KeyType, ValueType>>::const_iterator it) : it(it) {
        }
        const_iterator& operator++() {
            it++;
            return *this;
        }
        const_iterator& operator--() {
            it--;
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator save = *this;
            it++;
            return save;
        }
        const_iterator operator--(int) {
            const_iterator save = *this;
            it--;
            return save;
        }
        bool operator==(const const_iterator& b) {
            return (it == b.it);
        }
        bool operator!=(const const_iterator& b) {
            return (it != b.it);
        }
        const pair<const KeyType, ValueType>& operator*() const {
            return *it;
        }
        const typename list<pair<const KeyType, ValueType>>::const_iterator& operator->() const {
            return it;
        }
    };


    HashMap() : count(0) {
    }
    template <typename InputIterator>
    HashMap(InputIterator begin, InputIterator end) : count(0) {
        while(begin != end) {
            insert(*begin);
            begin++;
        }
    }
    HashMap(const initializer_list<pair<const KeyType, ValueType>> &list) : count(0) {
        for (auto elem: list) {
            insert(elem);
        }
    }
    HashMap(Hash hasher) : hasher(hasher), count(0) {
    }
    template <typename InputIterator>
    HashMap(InputIterator begin, InputIterator end, Hash hasher) : hasher(hasher), count(0) {
        while(begin != end) {
            insert(*begin);
            begin++;
        }
    }
    HashMap(const initializer_list<pair<const KeyType, ValueType>> &list, Hash hasher) : hasher(hasher), count(0) {
        for (auto elem: list) {
            insert(elem);
        }
    }


    HashMap& operator=(const HashMap& b) {
        hasher = b.hasher;
        for (auto elem : b) {
            insert(elem);
        }
        return *this;
    }

    size_t size() const {
        return count;
    }

    bool empty() const {
        return (count == 0);
    }

    const Hash& hash_function() const {
        return hasher;
    }

    void insert(const pair<const KeyType, ValueType>& elem) {
        if (find(elem.first) != elems.end()) {
            return;
        }
        count++;
        if (count > table.size() * 0.75) {
            elems.push_back(elem);
            size_t cur_size = table.size();
            table.resize(ALPHA * (cur_size + MIN_SIZE));
            rebuild();
        } else {
            size_t hashed_key = hasher(elem.first) % table.size();
            typename list<pair<const KeyType, ValueType>>::iterator help = elems.insert(table[hashed_key].first, elem);
            if (table[hashed_key].second == elems.end()) {
                table[hashed_key].first = help;
                table[hashed_key].second = help;
            } else {
                table[hashed_key].first = help;
            }
        }
    }

    void erase(const KeyType key) {
        if (table.size() == 0) {
            return;
        }
        size_t hashed_key = hasher(key) % table.size();
        if (table[hashed_key].second == elems.end()) {
            return;
        }
        if (table[hashed_key].second == table[hashed_key].first) {
            if (table[hashed_key].second->first == key) {
                elems.erase(table[hashed_key].second);
                table[hashed_key].second = elems.end();
                table[hashed_key].first = elems.end();
                count--;
                return;
            } else {
                return;
            }
        }
        auto it = table[hashed_key].first;
        it++;
        for (; it != table[hashed_key].second; it++) {
            if (it->first == key) {
                elems.erase(it);
                count--;
                return;
            }
        }
        if (table[hashed_key].first->first == key) {
            auto it = elems.erase(table[hashed_key].first);
            count--;
            table[hashed_key].first = it;
        }
        if (table[hashed_key].second->first == key) {
            auto it = elems.erase(table[hashed_key].second);
            count--;
            it--;
            table[hashed_key].second = it;
        }
    }

    iterator begin() {
        return iterator(elems.begin());
    }
    iterator end() {
        return iterator(elems.end());
    }
    const_iterator end() const {
        return const_iterator(elems.end());
    }
    const_iterator begin() const {
        return const_iterator(elems.begin());
    }

    iterator find(const KeyType& key) {
        if (count == 0) {
            return end();
        }
        size_t hashed_key = hasher(key) % table.size();
        if (table[hashed_key].second == elems.end()) {
            return end();
        }
        for (auto it = table[hashed_key].first; it != table[hashed_key].second; it++) {
            if (it->first == key) {
                return iterator(it);
            }
        }
        if (table[hashed_key].second->first == key) {
            return iterator(table[hashed_key].second);
        }
        return end();
    }
    const_iterator find(const KeyType& key) const {
        if (count == 0) {
            return end();
        }
        size_t hashed_key = hasher(key) % table.size();
        if (table[hashed_key].second == elems.end()) {
            return end();
        }
        for (auto it = table[hashed_key].first; it != table[hashed_key].second; it++) {
            if (it->first == key) {
                return const_iterator(it);
            }
        }
        if (table[hashed_key].second->first == key) {
            return const_iterator(table[hashed_key].second);
        }
        return end();
    }

    ValueType& operator[](const KeyType& key) {
        if (find(key) == elems.end()) {
            insert({key, ValueType()});
        }
        size_t hashed_key = hasher(key) % table.size();
        for (auto it = table[hashed_key].first; it != table[hashed_key].second; it++) {
            if (it->first == key) {
                return it->second;
            }
        }
        return table[hashed_key].second->second;
    }

    const ValueType& at(const KeyType& key) const {
        if (table.size() == 0) {
            throw std::out_of_range("");
        }
        size_t hashed_key = hasher(key) % table.size();
        for (auto it = table[hashed_key].first; it != table[hashed_key].second; it++) {
            if (it->first == key) {
                return it->second;
            }
        }
        if (table[hashed_key].second->first == key) {
            return table[hashed_key].second->second;
        } else {
            throw std::out_of_range("");
        }
    }

    void clear() {
        list<pair<KeyType, ValueType>> save;
        for (auto elem : elems) {
            save.insert(save.begin(), elem);
        }
        for (auto elem: save) {
            erase(elem.first);
        }
    }
};
