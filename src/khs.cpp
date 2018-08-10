#include "khset.h"
#include <cmath>

int main(int argc, char *argv[]) {
    const size_t NSUM = argc == 1 ? 100000000: std::strtoull(argv[1], nullptr, 10);
    khset64_t khis, kh2;
    for(size_t i(0); i < NSUM << 1; ++i)
        khis.insert(i), kh2.insert(NSUM - i);
    {
        khset64_t kh3(std::move(kh2));
        std::fprintf(stderr, "size of kh3: %zu\n", kh3.size());
        khis.for_each([&](const auto key){kh3.insert(key);});
        std::fprintf(stderr, "size of unionized: %zu\n", kh3.size());
        khset64_t kh4(kh3);
        std::fprintf(stderr, "size of kh4: %zu. (kh3 is still %zu)\n", kh4.size(), kh3.size());
    }
    khset32_t kh1;
    while(kh1.size() < NSUM) kh1.insert(kh1.size() * (1 + kh1.size()) * (2 + khis.size()));
    std::fprintf(stderr, "kh1 size: %zu\n", kh1.size());
    khset32_t khc(kh1);
    khset64_t kh3;
    while(kh3.size() < NSUM) kh3.insert(std::pow(kh3.size(), 2));
    std::fprintf(stderr, "kh3 size: %zu\n", kh3.size());
    khis += kh2;
    khis += kh3;
    std::fprintf(stderr, "khis size: %zu\n", khis.size());
}
