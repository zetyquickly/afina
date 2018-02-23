#ifndef AFINA_STORAGE_MAP_BASED_GLOBAL_LOCK_IMPL_H
#define AFINA_STORAGE_MAP_BASED_GLOBAL_LOCK_IMPL_H

#include <map>
#include <mutex>
#include <string>

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
    ~MapBasedGlobalLockImpl() {}

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

    size_t _max_size;
    size_t _cur_size;
    std::map<std::string, Entry *> _backend;
    Entry mutable *head;
    Entry mutable *tail;

};

} // namespace Backend
} // namespace Afina

#endif // AFINA_STORAGE_MAP_BASED_GLOBAL_LOCK_IMPL_H
