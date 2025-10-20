#include "compress.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <zlib.h> 

const int CHUNK_SIZE = 16384; // 16KB buffer


void check_zlib_err(int ret, const char* msg) {
    if (ret != Z_OK) {
        std::cerr << "zlib error: " << msg << " (" << ret << ")" << std::endl;
    }
}

bool compress_file(const fs::path& input_file, const fs::path& output_file) {
    std::ifstream ifs(input_file, std::ios::binary);
    if (!ifs) {
        std::cerr << "Error: Cannot open input file for compression: " << input_file << std::endl;
        return false;
    }

    std::ofstream ofs(output_file, std::ios::binary);
    if (!ofs) {
        std::cerr << "Error: Cannot create output file for compression: " << output_file << std::endl;
        return false;
    }

    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    int ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
    if (ret != Z_OK) {
        check_zlib_err(ret, "deflateInit failed");
        return false;
    }

    std::vector<char> in_buffer(CHUNK_SIZE);
    std::vector<char> out_buffer(CHUNK_SIZE);

    int flush;
    do {
        ifs.read(in_buffer.data(), CHUNK_SIZE);
        strm.avail_in = ifs.gcount();
        strm.next_in = reinterpret_cast<Bytef*>(in_buffer.data());

        flush = ifs.eof() ? Z_FINISH : Z_NO_FLUSH;

        do {
            strm.avail_out = CHUNK_SIZE;
            strm.next_out = reinterpret_cast<Bytef*>(out_buffer.data());
            ret = deflate(&strm, flush);
            check_zlib_err(ret, "deflate failed");
            
            int have = CHUNK_SIZE - strm.avail_out;
            ofs.write(out_buffer.data(), have);

        } while (strm.avail_out == 0);

    } while (flush != Z_FINISH);

    deflateEnd(&strm);
    return true;
}

bool decompress_file(const fs::path& input_file, const fs::path& output_file) {
    std::ifstream ifs(input_file, std::ios::binary);
    if (!ifs) {
        std::cerr << "Error: Cannot open input file for decompression: " << input_file << std::endl;
        return false;
    }

    std::ofstream ofs(output_file, std::ios::binary);
    if (!ofs) {
        std::cerr << "Error: Cannot create output file for decompression: " << output_file << std::endl;
        return false;
    }

    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;

    int ret = inflateInit(&strm);
    if (ret != Z_OK) {
        check_zlib_err(ret, "inflateInit failed");
        return false;
    }

    std::vector<char> in_buffer(CHUNK_SIZE);
    std::vector<char> out_buffer(CHUNK_SIZE);

    do {
        ifs.read(in_buffer.data(), CHUNK_SIZE);
        strm.avail_in = ifs.gcount();
        if (strm.avail_in == 0) break;
        strm.next_in = reinterpret_cast<Bytef*>(in_buffer.data());

        do {
            strm.avail_out = CHUNK_SIZE;
            strm.next_out = reinterpret_cast<Bytef*>(out_buffer.data());
            ret = inflate(&strm, Z_NO_FLUSH);
            check_zlib_err(ret, "inflate failed");

            if(ret == Z_STREAM_ERROR) {
                inflateEnd(&strm);
                return false;
            }

            int have = CHUNK_SIZE - strm.avail_out;
            ofs.write(out_buffer.data(), have);

        } while (strm.avail_out == 0);

    } while (ret != Z_STREAM_END);

    inflateEnd(&strm);
    return ret == Z_STREAM_END;
}