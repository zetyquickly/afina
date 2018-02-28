#include "MapBasedGlobalLockImpl.h"

#include <iostream>
#include <mutex>

namespace Afina {
namespace Backend {


// See MapBasedGlobalLockImpl.h
bool MapBasedGlobalLockImpl::Put(const std::string &key, const std::string &value) {
    size_t len;
    len = key.size() + value.size();
    if (len > _max_size) {
        return false;
    }
    while (_max_size - _cur_size < len) {
        Delete(tail->_key);
    }
    std::map<str, Entry *>::iterator got = _backend.find(key);
    if (got == _backend.end()) {
        Entry *entry = new Entry(key, value, head, nullptr);
        if (tail == nullptr) {
            tail = entry;
        }
        head = entry;
        if (head != tail) {
            head->_next->_prev = head;
        }
        _backend.insert(std::pair<str, Entry *>(entry->_key, entry));
    } else {
        got->second->_value = value;
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
    _cur_size += len;
    return true;
}

// See MapBasedGlobalLockImpl.h
bool MapBasedGlobalLockImpl::PutIfAbsent(const std::string &key, const std::string &value) {
    size_t len;
    len = key.size() + value.size();
    if (len > _max_size) {
        return false;
    }
    std::map<str, Entry *>::iterator got = _backend.find(key);
    if (got == _backend.end()) {
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
        _backend.insert(std::pair<str, Entry *>(entry->_key, entry));
    } else {
        return false;
    }
    _cur_size += len;
    return true;
}

// See MapBasedGlobalLockImpl.h
bool MapBasedGlobalLockImpl::Set(const std::string &key, const std::string &value) {
    size_t len, last_value_len, key_len, value_len;
    key_len = key.size();
    value_len = value.size();
    len = key_len + value_len;
    if (len > _max_size) {
        return false;
    }
    std::map<str, Entry *>::iterator got = _backend.find(key);
    if (got != _backend.end()) {
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
        while (_max_size - _cur_size < len) {
            Delete(tail->_key);
        }
        last_value_len = got->second->_value.size();
        got->second->_value = value;
    } else {
        return false;
    }
    _cur_size += value_len - last_value_len;
    return true;
}

// See MapBasedGlobalLockImpl.h
bool MapBasedGlobalLockImpl::Delete(const std::string &key) {
    std::map<str, Entry *>::iterator got = _backend.find(key);
    size_t len;
    if (got != _backend.end()) {
        len = key.size() + got->second->_value.size();
        if (head == tail) {
            _backend.erase(got);
            delete got->second;
            head = nullptr;
            tail = nullptr;
            _cur_size = 0;
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
        return false;
    }
    _cur_size -= len;
    return true;
}

// See MapBasedGlobalLockImpl.h
bool MapBasedGlobalLockImpl::Get(const std::string &key, std::string &value) const {
    std::map<str, Entry *>::const_iterator got = _backend.find(key);
    if (got != _backend.end()) {
        value = got->second->_value;
        if (head = tail) {
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
        return false;
    }
    return true;
}

} // namespace Backend
} // namespace Afina
