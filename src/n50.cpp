#include "kseq.h"
#include "zlib.h"
#include <cstdint>
#include <cstdio>
#include <map>

KSEQ_INIT(gzFile, gzread)

using std::size_t;

size_t n50(const char *path) {
    gzFile fp = gzopen(path, "rb");
    if(!fp) throw 1;
    std::map<size_t, uint32_t> length_counts;
    kseq_t *kseq = kseq_init(fp);
    size_t total_contigs = 0;
    while(kseq_read(kseq) >= 0)
        ++length_counts[kseq->seq.l], ++total_contigs;
    size_t progress = 0; // Scientific progress goes boink!
    auto rit = length_counts.rbegin();
    while(rit != length_counts.rend() && progress + rit->second <= total_contigs / 2) {
        progress += rit->second;
    }
    size_t ret = rit->first;
    kseq_destroy(kseq);
    gzclose(fp);
    return ret;
}

#ifdef MAIN_FN
int main(int argc, char *argv[]) {
    for(auto it = argv; *it; ++it) {
        std::fprintf(stderr, "Genome %s has %zu for n50\n", *it, n50(*it));
    }
}
#endif
