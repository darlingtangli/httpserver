#ifndef COMPRESSFILTER_H
#define COMPRESSFILTER_H
#include <zlib.h>
#include <string>
class CompressFilter {
    public:
    enum METHOD {GZIP=1, COMPRESS=2, DEFLATE=3};
    enum STATUS {COM=1, DECOM=2, OTHER=3};
    CompressFilter() {
        _comstat = OTHER;
        _finish = false;
    }
    bool compressInit(METHOD m);
    bool decompressInit(METHOD m);
    bool compress(char* in, unsigned int len, std::string& out, bool finish);
    bool decompress(char* in, unsigned int len, std::string& out, bool finish);
    bool finish();
    private:
    z_stream _strm;
    METHOD _method;
    STATUS _comstat;
    bool _status;
    bool _finish;
};
#endif
