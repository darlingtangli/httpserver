#include "CompressFilter.h"
#define CHUNK 1000
bool CompressFilter::compressInit(METHOD m) {
    _status = true;
    _finish = false;
    _comstat = COM;
    int ret;
    _method = m;
    switch (m) {
    case GZIP:
        break;
    case COMPRESS:
        break;
    case DEFLATE:
        _strm.zalloc = Z_NULL;
        _strm.zfree = Z_NULL;
        _strm.opaque = Z_NULL;
        ret = deflateInit(&_strm, Z_DEFAULT_COMPRESSION);
      /*
        ret = deflateInit2(&_strm,
                           Z_DEFAULT_COMPRESSION,
                           Z_DEFLATED,
                           -MAX_WBITS,
                           8,
                           Z_DEFAULT_STRATEGY);*/
        if (ret != Z_OK) {
            _status = false;
        }
        break;
    }
    return _status;
}
bool CompressFilter::decompressInit(METHOD m) {
    _status = true;
    _finish = false;
    _comstat = DECOM;
    int ret;
    _method = m;
    switch (m) {
    case GZIP:
        break;
    case COMPRESS:
        break;
    case DEFLATE:
        _strm.zalloc = Z_NULL;
        _strm.zfree = Z_NULL;
        _strm.opaque = Z_NULL;
        _strm.avail_in = 0;
        _strm.next_in = Z_NULL;
        ret = inflateInit(&_strm);
        if (ret != Z_OK) {
            _status = false;
        }
        break;
    }
    return _status;

}
bool CompressFilter::compress(char* in, unsigned int len, std::string& out, bool finish) {
    if (!_status)
        return false;

    char* temp;
    int ret;
    int buflen;
    int flush;
    out = "";
    switch (_method) {
    case GZIP:
        break;
    case COMPRESS:
        break;
    case DEFLATE:
        _strm.next_in = (Bytef*)in;
        _strm.avail_in = len;
        if (len>CHUNK) {
            buflen = len;
        } else {
            buflen = CHUNK;
        }

        temp = new char[buflen];
        flush = finish? Z_FINISH:Z_NO_FLUSH;
        do {
            _strm.avail_out = buflen;
            _strm.next_out = (Bytef*)temp;
            ret = deflate(&_strm, flush);    /* no bad return value */
            if (ret == Z_STREAM_ERROR) {
                deflateEnd(&_strm);
                delete temp;
                temp = NULL;
                return false;
            }
            out.append(temp, buflen-_strm.avail_out);
        } while (_strm.avail_out == 0);
        if (finish) {
            deflateEnd(&_strm);
            if (ret != Z_STREAM_END) {
                delete temp;
                temp = NULL;
                return false;
            }
        }
        delete []temp;
        break;
    }
    return true;
}
bool CompressFilter::decompress(char* in, unsigned int len, std::string& out, bool finish) {
    if (!_status)
        return false;
    char* temp;
    int ret;
    int buflen;
    int flush;
    out = "";
    switch (_method) {
    case GZIP:
        break;
    case COMPRESS:
        break;
    case DEFLATE:
        _strm.next_in = (Bytef*)in;
        _strm.avail_in = len;
        if (len>CHUNK) {
            buflen = len;
        } else {
            buflen = CHUNK;
        }
        temp = new char[buflen];

        flush = finish? Z_FINISH:Z_NO_FLUSH;
        do {
            _strm.avail_out = buflen;
            _strm.next_out = (Bytef*)temp;
            ret = inflate(&_strm, Z_NO_FLUSH);    /* no bad return value */
            if (ret == Z_STREAM_ERROR || ret == Z_NEED_DICT || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
                inflateEnd(&_strm);
                delete []temp;
                return false;
            }
            out.append(temp, buflen-_strm.avail_out);
        } while (_strm.avail_out == 0);
        if (finish) {
            inflateEnd(&_strm);
            if (ret != Z_STREAM_END) {
                delete []temp;
                return false;
            }
        }
        delete []temp;
        break;
    }

    return true;
}
bool CompressFilter::finish() {
    if ((!_finish) || (!_status))
        return false;
    switch (_method) {
    case GZIP:
        break;
    case COMPRESS:
        break;
    case DEFLATE:
        if (_comstat == COM)
            deflateEnd(&_strm);
        else if (_comstat == DECOM)
            inflateEnd(&_strm);
        break;
    }

    return true;
}
