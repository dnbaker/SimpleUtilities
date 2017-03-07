#include <gzfp.h>
#include "kstring.h"

template<typename T>
using free_ptr = std::unique_ptr<T, decltype(&free)>;

int main(int argc, char *argv[]) {
    int delim(0), colnum(2);
    if(argc < 2) {
        //usage:
        std::fprintf(stderr, "Usage: %s file.txt <colnum> <delim>\n", *argv);
        exit(1);
    }
    gzfp::GZFP fp(argv[1]);
    if(argc > 2) colnum = atoi(optarg);
    if(argc > 3) delim = atoi(argv[3]);
    char *line;
    int m(8);
    free_ptr<int> offsets((int *)malloc(m * sizeof(int)), free);
    int *const &t = offsets.get();
    int noff, ncols;
    while((line = fp.next())) {
        if((ncols = ksplit_core(line, delim, &noff, (int **)&t)) > colnum)
            fputs(line + t[colnum], stdout);
    }
}
