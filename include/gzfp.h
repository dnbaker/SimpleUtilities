#ifndef _GZFP_H
#define _GZFP_H
#include <cassert>
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <zlib.h>

#define __gzstream_open(fname, mode) \
    ((strcmp(fname, "-") == 0 || strcmp(fname, "stdin") == 0 || strcmp(fname, "stdout") == 0) \
        ? (gzdopen(*mode == 'r' ? STDIN_FILENO: STDOUT_FILENO, mode)) \
        : gzopen(fname, mode))

namespace gzfp {

static gzFile open_gzfile(const char *fname, const char *mode) {
    // If not set, zlib compresses all our files enormously.
    if(mode[1] == 'T' || (mode[1] == 'b' && mode[2] == '0')) {
        char omode[4];
#if ZLIB_VERNUM < 0x1260
#pragma message("Note: zlib version < 1.2.6 doesn't support transparent file writing. Writing uncompressed gzip files by default. Your version: " ZLIB_VERSION)
        std::fprintf(stderr, "Loading version " ZLIB_VERSION "\n");
        sprintf(omode, (char *)"wb0");
#else
        sprintf(omode, (char *)"wT");
#endif
        return __gzstream_open(fname, omode);
    }
    return __gzstream_open(fname, mode);
}

struct GZFP {
    gzFile fp_;
public:
    ssize_t linelen_;
    bool is_write_;
    std::uint8_t level_;
    int linesz_;
    std::unique_ptr<char []> buf_;
#if 0
    GZFP(GZFP &&other)
    {
        using std::swap;
        swap(*this, other);
    }
#endif
    void set_bufsz(unsigned bufsz) {
#if ZLIB_VERNUM < 0x1235
        std::fprintf(stderr, "Warning: gzbuffer added in zlib1.2.3.5. Unable to change "
                             "buffer size from default of 8192.\n");
#else
        gzbuffer(fp_, bufsz);
#endif
    }
    int getc() {
        return gzgetc(fp_);
    }
    void write(const char c) {
        assert(is_write_);
        gzputc(fp_, c);
    }
    void write(const char *s) {
        assert(is_write_);
        gzputs(fp_, s);
    }
    void printf(const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        gzprintf(fp_, fmt, args);
        va_end(args);
    }
    char *next() {
        return gzgets(fp_, buf_.get(), linesz_);
    }
    std::uint8_t infer_level(const char *s) const {
#if !NDEBUG
        const char *original_str(s);
#endif
        for(;*s;++s) {
            switch(*s) {
                case 'r': case 'T': return 0;
                default: if(*s >= '0' && *s <= '9') return *s - '0';
            }
        }
#if !NDEBUG
        std::fprintf(stderr, "Could not infer compression mode from mode string '%s'.\n", original_str);
#endif
        return 0; // Could not infer....
    }
    GZFP():
        fp_(nullptr),
        linelen_(0L),
        is_write_(0),
        level_(0),
        linesz_(0),
        buf_(nullptr)
    {
    }
    GZFP(gzFile fp, const char *mode, int linesz=2048):
        fp_(fp),
        linelen_(0L),
        is_write_(strchr(mode, 'w')),
        level_(infer_level(mode)),
        linesz_(is_write_ ? 0: linesz),
        buf_(std::make_unique<char []>(linesz))
    {
    }
    GZFP(const char *path, const char *mode, int linesz=2048): GZFP(open_gzfile(path, mode), mode, linesz) {} // File path
    GZFP(int fd, const char *mode, int linesz=2048): GZFP(gzdopen(fd, mode), mode, linesz) {} // File descriptor
    GZFP(FILE *fp, const char *mode, int linesz=2048): GZFP(fileno(fp), mode, linesz) {}      // File pointer
    GZFP(const char *path): GZFP(path, "r") {}
    ~GZFP() {
        if(fp_) gzclose(fp_);
    }
};

} // namespace mehmm

#endif // #ifdef _GZFP_H
