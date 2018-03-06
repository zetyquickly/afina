#ifndef AFINA_STORAGE_MAP_BASED_GLOBAL_LOCK_IMPL_H
#define AFINA_STORAGE_MAP_BASED_GLOBAL_LOCK_IMPL_H

#include <map>
#include <mutex>
#include <string>
#include <functional>
#include <iostream>

#include <afina/Storage.h>

namespace Afina {
namespace Backend {

/**
 * # Map based implementation with global lock
 *
 *
 */

class MapBasedGlobalLockImpl : public Afina::Storage {
public:
    MapBasedGlobalLockImpl(size_t max_size = 1024, size_t cur_size = 0)
        : _max_size(max_size), _cur_size(cur_size), head(nullptr), tail(nullptr) {}
    ~MapBasedGlobalLockImpl() {
        for (my_map::iterator it = _backend.begin(); it != _backend.end(); it++) {
            delete it->second;
        }
    }

    // Implements Afina::Storage interface
    bool Put(const std::string &key, const std::string &value) override;

    // Implements Afina::Storage interface
    bool PutIfAbsent(const std::string &key, const std::string &value) override;

    // Implements Afina::Storage interface
    bool Set(const std::string &key, const std::string &value) override;

    // Implements Afina::Storage interface
    bool Delete(const std::string &key) override;

    // Implements Afina::Storage interface
    bool Get(const std::string &key, std::string &value) const override;

private:
    struct Entry;
    using Entry = struct Entry {
        std::string _key;
        std::string _value;

        struct Entry *_next;
        struct Entry *_prev;

        Entry(std::string key, std::string value, Entry *next = nullptr, Entry *prev = nullptr)
            : _next(next), _prev(prev), _key(key), _value(value) {}
        ~Entry() {}
    };
    using str = const std::string;
    using str_ref = std::reference_wrapper<str>;
    using my_map = std::map<str_ref, Entry *, std::less<str>>;
    size_t _max_size;
    size_t _cur_size;
    my_map _backend;
    Entry mutable *head;
    Entry mutable *tail;
    std::recursive_mutex mutable mut;

};

} // namespace Backend
} // namespace Afina

#endif // AFINA_STORAGE_MAP_BASED_GLOBAL_LOCK_IMPL_H
