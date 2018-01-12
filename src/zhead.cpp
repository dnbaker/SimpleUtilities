#include <include/gzfp.h>

void zhead_usage(char *ex) {
    std::fprintf(stderr, "Application: Drop-in replacement for coreutils head, but transparently works on gzipped files.\n"
                         "Usage: %s <file>\nOmit file to read from stdin.\nFlags;\n-n: Number of lines. Default: 10.\n"
                         "-s: Number of lines to skip before emitting. Default: 0.\n"
                         "-c: Skip and emit characters rather than lines.\n"
                         , ex);
}

int main(int argc, char *argv[]) {
    int c;
    std::uint64_t n(10), skip(0);
    bool by_character(false);
    char *outpath(nullptr);
    while((c = getopt(argc, argv, "s:o:n:ch?")) >= 0) {
        switch(c) {
            case 'h': case '?': zhead_usage(argv[0]); exit(EXIT_FAILURE);
            case 'n': n = strtoull(optarg, nullptr, 10); break;
            case 's': skip = strtoull(optarg, nullptr, 10); break;
            case 'o': outpath = optarg; break;
            case 'c': by_character = true;
        }
    }
    switch(argc - optind) {
        case 1: /* do nothing */ break;
        case 0: 
            fprintf(stderr, "Reading from stdin.\n"); break;
            break; // Do nothing
        default: zhead_usage(*argv); return EXIT_FAILURE;
    }

    gzfp::GZFP fp(argc - optind ? argv[optind]: "-"); // Open handles.
    FILE *ofp(outpath?fopen(outpath, "w"):stdout);
    char *line;

    if(by_character) {
        while(skip-- && (c = fp.getc()) != EOF); // Skip characters
        while((c = fp.getc()) != EOF) std::fputc(c, ofp);
    } else {
        while(skip-- && (line = fp.next())); // Skip lines
        if(line)
            while(n-- && (line = fp.next()))
                fputs(line, ofp); // Read the number desired.
    }
    if(ofp != stdout) fclose(ofp); // Clean up
    return EXIT_SUCCESS;
}
