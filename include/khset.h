#pragma once
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include "klib/khash.h"

KHASH_SET_INIT_INT(set)
KHASH_SET_INIT_INT64(set64)

using u32 = ::std::uint32_t;
using u64 = ::std::uint64_t;

#define __FE__ \
    template<typename Functor>\
    void for_each(const Functor &func) {\
        for(khiter_t ki = 0; ki < this->n_buckets; ++ki)\
            if(kh_exist(this, ki))\
                func(this->keys[ki]);\
    }\
    template<typename Functor>\
    void for_each(const Functor &func) const {\
        for(khiter_t ki = 0; ki < this->n_buckets; ++ki)\
            if(kh_exist(this, ki))\
                func(this->keys[ki]);\
    }


// Steal everything, take no prisoners.
#define MOVE_DEC(t) \
   t(t &&other) {std::memcpy(this, &other, sizeof(*this)); std::memset(&other, 0, sizeof(other));}

#define COPY_DEC(t) \
    t(const t &other) {\
        if(other.size()) {\
            std::memcpy(this, &other, sizeof(*this));\
            auto memsz = other.size() * sizeof(*keys);\
            keys = static_cast<decltype(keys)>(std::malloc(memsz));\
            if(!keys) throw std::bad_alloc();\
            std::memcpy(keys, other.keys, memsz);\
            memsz = __ac_fsize(other.size()) * sizeof(u32);\
            flags = static_cast<u32 *>(std::malloc(memsz));\
            if(!flags) throw std::bad_alloc();\
            std::memcpy(flags, other.flags, memsz);\
        } else std::memset(this, 0, sizeof(*this));\
    }


struct khset32_t: khash_t(set) {
    khset32_t() {std::memset(this, 0, sizeof(*this));}
    ~khset32_t() {std::free(this->flags); std::free(this->keys);}
    COPY_DEC(khset32_t)
    MOVE_DEC(khset32_t)
    // For each
    __FE__
    operator khash_t(set) &() {return *reinterpret_cast<khash_t(set) *>(this);}
    operator khash_t(set) *() {return reinterpret_cast<khash_t(set) *>(this);}
    operator const khash_t(set) &() const {return *reinterpret_cast<const khash_t(set) *>(this);}
    operator const khash_t(set) *() const {return reinterpret_cast<const khash_t(set) *>(this);}
    khash_t(set) *operator->() {return static_cast<khash_t(set) *>(*this);}
    const khash_t(set) *operator->() const {return static_cast<const khash_t(set) *>(*this);}
    auto insert(u32 val) {
        int khr;
        auto ret = kh_put(set64, *this, val, &khr);
        if(khr < 0) throw std::bad_alloc();
        return ret;
    }
    template<typename ItType>
    void insert(ItType i1, ItType i2) {
        while(i1 < i2) insert(*i1++);
    }
    void clear() {kh_clear(set, this);}
    bool contains(u32 x) const {return kh_get(set, this, x) != kh_end(this);}
    size_t size() const {return kh_size(static_cast<const khash_t(set) *>(this));}
};

struct khset64_t: khash_t(set64) {
    khset64_t() {std::memset(this, 0, sizeof(*this));}
    ~khset64_t() {std::free(this->flags); std::free(this->keys);}
    COPY_DEC(khset64_t)
    MOVE_DEC(khset64_t)
    // For each
    __FE__
    operator khash_t(set64) &() {return *reinterpret_cast<khash_t(set64) *>(this);}
    operator khash_t(set64) *() {return reinterpret_cast<khash_t(set64) *>(this);}
    operator const khash_t(set64) &() const {return *reinterpret_cast<const khash_t(set64) *>(this);}
    operator const khash_t(set64) *() const {return reinterpret_cast<const khash_t(set64) *>(this);}
    khash_t(set64) *operator->() {return static_cast<khash_t(set64) *>(*this);}
    const khash_t(set64) *operator->() const {return static_cast<const khash_t(set64) *>(*this);}
    auto insert(u64 val) {
        int khr;
        auto ret = kh_put(set64, *this, val, &khr);
        if(khr < 0) throw std::bad_alloc();
        return ret;
    }
    void clear() {kh_clear(set64, this);}
    bool contains(u64 x) const {return kh_get(set64, this, x) != kh_end(this);}
    size_t size() const {return kh_size(static_cast<const khash_t(set64) *>(this));}
};
template<typename T> T&operator+=(T &a, const T &b) {
    b.for_each([&](auto k){a.insert(k);});
    return a;
}
#undef __FE__
#undef COPY_DEC
#undef MOVE_DEC
