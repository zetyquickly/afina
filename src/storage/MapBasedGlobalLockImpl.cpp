#include "MapBasedGlobalLockImpl.h"

#include <iostream>
#include <mutex>

namespace Afina {
namespace Backend {


// See MapBasedGlobalLockImpl.h
bool MapBasedGlobalLockImpl::Put(const std::string &key, const std::string &value) {
    mut.lock();
    size_t len, last_value_len, key_len, value_len;
    // more complex case when changes only the value length
    key_len = key.size();
    value_len = value.size();
    len = key_len + value_len;
    // if key + val greater than storage then stop
    if (len > _max_size) {
        mut.unlock();
        return false;
    }
    my_map::iterator got = _backend.find(key);
    // if the key is not in the map then add new entry by that key 
    if (got == _backend.end()) {
        // if there's not enough space then pop tail element 
        while (_max_size - _cur_size < len) {
            Delete(tail->_key);
        }
        Entry *entry = new Entry(key, value, head, nullptr);
        // in case if the list is empty
        if (tail == nullptr) {
            tail = entry;
        }
        // in all cases
        head = entry;
        // in case if the list wasn't empty
        if (head != tail) {
            head->_next->_prev = head;
        }
        _backend.insert(std::pair<str_ref, Entry *>(entry->_key, entry));
    // if the key is already in map rewrite entry's value field
    } else {
        last_value_len = got->second->_value.size();
        // if there's not enough space only for the value then pop tail element 
        while (_max_size - _cur_size - last_value_len < value_len) {
            Delete(tail->_key);
        }
        got->second->_value = value;
        // decrease current size on last value's length
        _cur_size -= last_value_len;
        // and place this very entry to the front of the list
        if (got->second != head) {
            if (got->second == tail) {
                got->second->_prev->_next = nullptr;
                tail = got->second->_prev;
            } else {
                got->second->_next->_prev = got->second->_prev;
                got->second->_prev->_next = got->second->_next;
            }
            got->second->_next = head;
            head->_prev = got->second;
            head = got->second;
        }
    }
    // increase current size counter
    _cur_size += len;
    mut.unlock();
    return true;
}

// See MapBasedGlobalLockImpl.h
bool MapBasedGlobalLockImpl::PutIfAbsent(const std::string &key, const std::string &value) {
    mut.lock();
    size_t len;
    len = key.size() + value.size();
    // if key + val greater than storage then stop
    if (len > _max_size) {
        mut.unlock();
        return false;
    }
    my_map::iterator got = _backend.find(key);
    // if the key is not in the map then add new entry by that key 
    if (got == _backend.end()) {
        // if there's not enough space then pop tail element 
        while (_max_size - _cur_size < len) {
            Delete(tail->_key);
        }
        Entry *entry = new Entry(key, value, head, nullptr);
        if (tail == nullptr) {
            tail = entry;
        }
        head = entry;
        if (head != tail) {
            head->_next->_prev = head;
        }
        _backend.insert(std::pair<str_ref, Entry *>(entry->_key, entry));
    } else {
        mut.unlock();
        return false;
    }
    _cur_size += len;
    mut.unlock();
    return true;
}

// See MapBasedGlobalLockImpl.h
bool MapBasedGlobalLockImpl::Set(const std::string &key, const std::string &value) {
    mut.lock();
    size_t len, last_value_len, key_len, value_len;
    // more complex case when changes only the value length
    key_len = key.size();
    value_len = value.size();
    len = key_len + value_len;
    // if key + val greater than storage then stop
    if (len > _max_size) {
        mut.unlock();
        return false;
    }
    my_map::iterator got = _backend.find(key);
    // if the key is already in map rewrite entry's value field
    if (got != _backend.end()) {
        // at first place the entry to the front
        if (got->second != head) {
            if (got->second == tail) {
                got->second->_prev->_next = nullptr;
                tail = got->second->_prev;
            } else {
                got->second->_next->_prev = got->second->_prev;
                got->second->_prev->_next = got->second->_next;
            }
            got->second->_next = head;
            head->_prev = got->second;
            head = got->second;
        }
        last_value_len = got->second->_value.size();
        // if there's not enough space for a new value then pop some entries
        while (_max_size - _cur_size - last_value_len < value_len) {
            Delete(tail->_key);
        }
        got->second->_value = value;
    } else {
        mut.unlock();
        return false;
    }
    _cur_size += value_len - last_value_len;
    mut.unlock();
    return true;
}

// See MapBasedGlobalLockImpl.h
bool MapBasedGlobalLockImpl::Delete(const std::string &key) {
    mut.lock();
    my_map::iterator got = _backend.find(key);
    size_t len;
    // if there's the key then remove
    if (got != _backend.end()) {
        len = key.size() + got->second->_value.size();
        // in case when there's only one entry in list
        if (head == tail) {
            _backend.erase(got);
            // safely free memory
            delete got->second;
            head = nullptr;
            tail = nullptr;
            _cur_size = 0;
            mut.unlock();
            return true;
        }
        if (got->second != head) {
            got->second->_prev->_next = got->second->_next;
        } else {
            head = got->second->_next;
            head->_prev = nullptr;
        }
        if (got->second != tail) {
            got->second->_next->_prev = got->second->_prev;
        } else {
            tail = got->second->_prev;
            tail->_next = nullptr;
        }
        _backend.erase(got);
        delete got->second;
    } else {
        mut.unlock();
        return false;
    }
    _cur_size -= len;
    mut.unlock();
    return true;
}

// See MapBasedGlobalLockImpl.h
bool MapBasedGlobalLockImpl::Get(const std::string &key, std::string &value) const {
    mut.lock();
    my_map::const_iterator got = _backend.find(key);
    // if there's the key then place it to the front and write the value of entry to value parameter
    if (got != _backend.end()) {
        value = got->second->_value;
        if (head = tail) {
            mut.unlock();
            return true;
        }
        if (got->second != head) {
            if (got->second == tail) {
                got->second->_prev->_next = nullptr;
                tail = got->second->_prev;
            } else {
                got->second->_next->_prev = got->second->_prev;
                got->second->_prev->_next = got->second->_next;
            }
            got->second->_next = head;
            head->_prev = got->second;
            head = got->second;
        }
    } else {
        mut.unlock();
        return false;
    }
    mut.unlock();
    return true;
}

} // namespace Backend
} // namespace Afina
