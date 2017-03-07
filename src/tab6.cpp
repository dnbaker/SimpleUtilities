#include <zlib.h>
#include <cstdio>
#include <cstdio>
#include "klib/kstring.h"
#include "klib/kseq.h"

KSEQ_INIT(gzFile, gzread)
#define __add_if_abs(kseq, suf) \
    do {\
    if(memcmp(kseq->name.s + kseq->name.l - sizeof(suf) + 1, suf, sizeof(suf) - 1))\
        kputsn(suf, sizeof(suf), &kseq->name);\
    } while(0)

int stream_process(char *outpath) {
    FILE *out(outpath ? fopen(outpath, "w"): stdout);
    gzFile in(gzdopen(STDIN_FILENO, "rb"));
    kseq_t *ks(kseq_init(in));
    kstring_t s{0, 0, nullptr};
    int code(EXIT_SUCCESS);
    for(;;) {
        if(kseq_read(ks) < 0) break;
        s.l = 0;
        __add_if_abs(ks, "/1");
        kputsn(ks->name.s, ks->name.l, &s);
        s.s[s.l++] = '\t';
        kputsn(ks->seq.s,  ks->seq.l,  &s);
        s.s[s.l++] = '\t';
        kputsn(ks->qual.s, ks->qual.l, &s);
        s.s[s.l++] = '\t';
        if(kseq_read(ks) < 0) {
            std::fprintf(stderr, "Uneven number of reads.\n");
            code = EXIT_FAILURE;
            break;
        }
        __add_if_abs(ks, "/2");
        kputsn(ks->name.s, ks->name.l, &s);
        s.s[s.l++] = '\t';
        kputsn(ks->seq.s, ks->seq.l, &s);
        s.s[s.l++] = '\t';
        kputsn(ks->qual.s, ks->qual.l, &s);
        s.s[s.l++] = '\n';
        s.s[s.l] = '\0';
        fwrite(s.s, 1uL, s.l, out);
    }
    kseq_destroy(ks);
    free(s.s);
    if(out != stdout) fclose(out);
    return code;
}


int main(int argc, char *argv[])
{
    int c, streaming(0);
    char *outpath(nullptr);
    while((c = getopt(argc, argv, "o:sh?")) >= 0) {
        switch(c) {
            case 'h': case '?': goto usage;
            case 'o': outpath = optarg; break;
            case 's': streaming = 1; break;
        }
    }
    if(streaming) return stream_process(outpath);
    if(argc - optind < 2) {
        usage:
        std::fprintf(stderr,"Function: Converts paired-end sequencing data into tab6 format for consumption by bowtie2\n" 
                            "Usage: %s <r1> <r2>\nFlags:\n-o\tEmit to file instead of stdout.\n"
                            "-s\tUse in streaming fashion (reads from stdin, emits to stdout by default).\n",
                     *argv);
        exit(EXIT_FAILURE);
    }
    FILE *out(outpath ? fopen(outpath, "w"): stdout);
    gzFile in1(gzopen(argv[optind], "rb"));
    gzFile in2(gzopen(argv[optind + 1], "rb"));
    kseq_t *ks1(kseq_init(in1)), *ks2(kseq_init(in2));
    kstring_t s{0, 256uL, (char *)malloc(256uL)};
    while(kseq_read(ks1) >= 0 && kseq_read(ks2) >= 0) {
        __add_if_abs(ks1, "/1");
        __add_if_abs(ks2, "/2");
        s.l = 0;
        kputsn(ks1->name.s, ks1->name.l, &s);
        s.s[s.l++] = '\t';
        kputsn(ks1->seq.s, ks1->seq.l, &s);
        s.s[s.l++] = '\t';
        kputsn(ks1->qual.s, ks1->qual.l, &s);
        s.s[s.l++] = '\t';
        kputsn(ks2->name.s, ks2->name.l, &s);
        s.s[s.l++] = '\t';
        kputsn(ks2->seq.s, ks2->seq.l, &s);
        s.s[s.l++] = '\t';
        kputsn(ks2->qual.s, ks2->qual.l, &s);
        s.s[s.l++] = '\n';
        s.s[s.l] = '\0';
        fwrite(s.s, 1uL, s.l, out);
    }
    free(s.s);
    kseq_destroy(ks1);
    kseq_destroy(ks2);
    if(out != stdout) fclose(out);
    gzclose(in1);
    gzclose(in2);
    return EXIT_SUCCESS;
}

#undef __add_if_abs

