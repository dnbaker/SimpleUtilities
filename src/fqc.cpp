#include <zlib.h>
#include <cstdint>
#include <cinttypes>

#include "klib/kseq.h"
#include "klib/kstring.h"
KSEQ_INIT(gzFile, gzread)

#define LOG_EXIT(...) do {\
        fprintf(stderr, "[E: %s:%d", __func__, __LINE__);\
        fprintf(stderr, ##__VA_ARGS__); exit(1); \
    } while(0) 

int main(int argc, char **argv)
{
    if(argc == 1 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
        LOG_EXIT("Usage: %s <inpath> [inpath2] [inpath3] ...\n", argv[0]);
    fputs("#Filename\tCount\n", stdout);
    for(int i = 1; i < argc; ++i){
        gzFile in(gzopen(argv[i], "r"));
        if(in == nullptr) LOG_EXIT("Could not open file %s.\n", argv[1]);
        kseq_t *seq(kseq_init(in));
        if(seq == nullptr) LOG_EXIT("Could not open file %s as fastq.\n", argv[1]);
        std::uint64_t c(0);
        while(kseq_read(seq) >= 0) ++c;
        fprintf(stdout, "%s\t%" PRIu64 "\n", argv[i], c);
        kseq_destroy(seq);
        gzclose(in);
    }
    return 0;
}
