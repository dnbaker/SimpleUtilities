#include <stdio.h>
#include "kseq.h"
#include "zlib.h"

KSEQ_INIT(gzFile, gzread)

static const unsigned char lut[] = {
   0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 84, 66, 71, 68, 69, 70, 67, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 65, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 116, 98, 103, 100, 101, 102, 99, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 97, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255
};

int main(int argc, char *argv[]) {
    if(argc > 1 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        fprintf(stderr, "Usage: %s <input.fasta>\nOmitting an argument defaults to reading from stdin.\n", argv[0]);   
        return EXIT_FAILURE;
    }
    gzFile fp = (argc > 1) ?  gzopen(argv[1], "rb"): gzdopen(STDIN_FILENO, "rb");
    if(fp == 0) abort();
    if(argc == 1) {
        fprintf(stderr, "Note: no path to a fasta has been provided. Defaulting to reading from stdin.\n");
    }
    kseq_t *ks = kseq_init(fp);
    size_t i;
    while(kseq_read(ks) >= 0) {
        for(i = 0; i < ks->seq.l >> 1; ++i) {
            char tmp = ks->seq.s[i];
            ks->seq.s[i] = lut[ks->seq.s[ks->seq.l - i - 1]];
            ks->seq.s[ks->seq.l - i - 1] = lut[tmp];
        }
        if(ks->seq.l & 1) ks->seq.s[i] = lut[ks->seq.s[i]];
        if(ks->qual.s) {
            for(i = 0; i < ks->qual.l >> 1; ++i) {
                char tmp = ks->qual.s[i];
                ks->qual.s[i] = ks->qual.s[ks->qual.l - i - 1];
                ks->qual.s[ks->qual.l - i - 1] = tmp;
            }
        }
        if(ks->qual.s) {
            fprintf(stdout, "@%s", ks->name.s);
            if(ks->comment.s) fprintf(stdout, " %s", ks->comment.s);
            fputc('\n', stdout);
            fflush(stdout);
            write(STDOUT_FILENO, ks->seq.s, ks->seq.l);
            write(STDOUT_FILENO, "\n+\n", 3);
            write(STDOUT_FILENO, ks->qual.s, ks->qual.l);
            write(STDOUT_FILENO, "\n", 1);
        } else {
            fprintf(stdout, ">%s", ks->name.s);
            if(ks->comment.s) fprintf(stdout, " %s", ks->comment.s);
            fputc('\n', stdout);
            fflush(stdout);
            write(STDOUT_FILENO, ks->seq.s, ks->seq.l);
            write(STDOUT_FILENO, "\n", 1);
        }
    }
    kseq_destroy(ks);
    gzclose(fp);
}
