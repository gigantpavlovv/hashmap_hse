#include <iostream>
#include <list>
#include <vector>
#include <stdexcept>
using namespace std;

template<typename KeyType, typename ValueType, typename Hash = std::hash<KeyType> >
class HashMap{
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
        iterator(typename list<pair<const KeyType, ValueType>>::iterator _it) : it(_it) {
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
        const_iterator(typename list<pair<const KeyType, ValueType>>::const_iterator _it) : it(_it) {
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
    HashMap(Hash _hasher) : hasher(_hasher), count(0) {
    }
    template <typename InputIterator>
    HashMap(InputIterator begin, InputIterator end, Hash _hasher) : hasher(_hasher), count(0) {
        while(begin != end) {
            insert(*begin);
            begin++;
        }
    }
    HashMap(const initializer_list<pair<const KeyType, ValueType>> &list, Hash _hasher) : hasher(_hasher), count(0) {
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
            table.resize(2 * (cur_size + 1));
            rebuild();
        } else {
            size_t i = hasher(elem.first) % table.size();
            typename list<pair<const KeyType, ValueType>>::iterator help = elems.insert(table[i].first, elem);
            if (table[i].second == elems.end()) {
                table[i].first = help;
                table[i].second = help;
            } else {
                table[i].first = help;
            }
        }
    }
    void erase(const KeyType key) {
        //cout << "Y-1" << endl;
        if (table.size() == 0) {
            //cout << "Y0" << endl;
            return;
        }
        size_t i = hasher(key) % table.size();
        //cout << table[i].first->first << endl;
        //cout << table[i].second->first << endl;
        if (table[i].second == elems.end()) {
            //cout << "Y1" << endl;
            return;
        }
        if (table[i].second == table[i].first) {
            //cout << "Y2" << endl;
            if (table[i].second->first == key) {
                elems.erase(table[i].second);
                table[i].second = elems.end();
                table[i].first = elems.end();
                count--;
                return;
            } else {
                return;
            }
        }
        auto it = table[i].first;
        it++;
        for (; it != table[i].second; it++) {
            if (it->first == key) {
                //cout << "Y" << endl;
                elems.erase(it);
                count--;
                return;
            }
        }
        //cout << table[i].first->first << endl;
        if (table[i].first->first == key) {
            //cout << "U0" << endl;
            auto it = elems.erase(table[i].first);
            count--;
            table[i].first = it;
        }
        if (table[i].second->first == key) {
            //cout << "U!" << endl;
            auto it = elems.erase(table[i].second);
            count--;
            it--;
            table[i].second = it;
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
        //cout << "X" << endl;
        if (count == 0) {
            return end();
        }
        size_t i = hasher(key) % table.size();

        if (table[i].second == elems.end()) {
            return end();
        }
        for (auto it = table[i].first; it != table[i].second; it++) {
            if (it->first == key) {
                //cout << "Y" << endl;
                return iterator(it);
            }
        }
        //cout << "X " << table[i].second->first << ' ' << table[i].second->second << endl;
        if (table[i].second->first == key) {
            return iterator(table[i].second);
        }
        return end();
    }
    const_iterator find(const KeyType& key) const {
        if (count == 0) {
            return end();
        }
        size_t i = hasher(key) % table.size();
        if (table[i].second == elems.end()) {
            return end();
        }
        for (auto it = table[i].first; it != table[i].second; it++) {
            if (it->first == key) {
                return const_iterator(it);
            }
        }
        if (table[i].second->first == key) {
            return const_iterator(table[i].second);
        }
        return end();
    }
    ValueType& operator[](const KeyType& key) {
        if (find(key) == elems.end()) {
            insert({key, ValueType()});
        }
        size_t i = hasher(key) % table.size();
        for (auto it = table[i].first; it != table[i].second; it++) {
            if (it->first == key) {
                return it->second;
            }
        }
        return table[i].second->second;
    }
    const ValueType& at(const KeyType& key) const {
        if (table.size() == 0) {
            throw std::out_of_range("");
        }
        size_t i = hasher(key) % table.size();
        for (auto it = table[i].first; it != table[i].second; it++) {
            if (it->first == key) {
                return it->second;
            }
        }
        if (table[i].second->first == key) {
            return table[i].second->second;
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
