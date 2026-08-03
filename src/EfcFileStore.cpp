// Backward-compat: flag renamed EXTERNAL_FLASH_MODULE -> OPENKNX_EXTFLASH; old name still works.
#if defined(EXTERNAL_FLASH_MODULE) && !defined(OPENKNX_EXTFLASH)
    #define OPENKNX_EXTFLASH
#endif
#include "EfcFileStore.h"
#ifdef OPENKNX_EXTFLASH
    #include <string.h>
    #include "ExternalFlash.h"

namespace efc
{
    IFileStore fileStore;

    static File _src;
    static File _sink;
    static File _dir;

    bool IFileStore::available() { return extFlashModule.isMounted(); }
    uint64_t IFileStore::totalBytes()
    {
        FSInfo fi;
        if (!extFlashModule.info(fi)) return 0;
        return (uint64_t)fi.totalBytes;
    }
    uint64_t IFileStore::freeBytes()
    {
        FSInfo fi;
        if (!extFlashModule.info(fi)) return 0;
        return (uint64_t)fi.totalBytes - fi.usedBytes;
    }

    int32_t IFileStore::open(const char *path)
    {
        _src = extFlashModule.open(path, "r");
        if (!_src) return -1;
        return (int32_t)_src.size();
    }
    // 0 only at true EOF; retry transient reads (never 0 mid-file -> no silent truncation). Bounded.
    uint8_t IFileStore::read(uint32_t offset, uint8_t *buf, uint8_t len)
    {
        if (!_src || buf == nullptr || len == 0) return 0;
        if ((uint64_t)offset >= _src.size()) return 0;
        if (_src.position() != offset && !_src.seek(offset)) return 0;
        for (uint8_t attempt = 0; attempt < 4; ++attempt)
        {
            const int r = _src.read(buf, len);
            if (r > 0) return (uint8_t)r;
            if (!_src.seek(offset)) break;
        }
        return 0;
    }
    void IFileStore::close() { _src.close(); }

    bool IFileStore::exists(const char *path) { return extFlashModule.exists(path); }

    bool IFileStore::sinkOpen(const char *path, uint32_t offset)
    {
        _sink = extFlashModule.open(path, offset ? "r+" : "w");
        if (!_sink) return false;
        if (offset && !_sink.seek(offset))
        {
            _sink.close();
            return false;
        }
        return true;
    }
    int IFileStore::sinkWrite(const uint8_t *buf, uint16_t len)
    {
        if (!_sink || buf == nullptr || len == 0) return -1;
        return (int)_sink.write(buf, len);
    }
    int IFileStore::sinkWriteAt(uint32_t offset, const uint8_t *buf, uint16_t len)
    {
        if (!_sink || buf == nullptr || len == 0) return -1;
        if (_sink.position() != offset && !_sink.seek(offset)) return -1;
        return (int)_sink.write(buf, len);
    }
    void IFileStore::sinkClose() { _sink.close(); }

    bool IFileStore::dirOpen(const char *path)
    {
        _dir = extFlashModule.open((path && *path) ? path : "/", "r");
        return (bool)_dir;
    }
    uint8_t IFileStore::dirNext(char *nameOut, uint16_t cap, uint32_t *sizeOut)
    {
        if (!_dir || cap == 0) return 0;
        File e = _dir.openNextFile();
        if (!e) return 0;
        strncpy(nameOut, e.name(), cap - 1);
        nameOut[cap - 1] = '\0';
        const bool isDir = e.isDirectory();
        if (sizeOut) *sizeOut = isDir ? 0 : (uint32_t)e.size();
        e.close();
        return isDir ? 2 : 1;
    }
    void IFileStore::dirClose() { _dir.close(); }

    bool IFileStore::remove(const char *path) { return extFlashModule.remove(path); }
    bool IFileStore::mkdir(const char *path) { return extFlashModule.mkdir(path); }
    bool IFileStore::rmdir(const char *path) { return extFlashModule.rmdir(path); }
    bool IFileStore::rename(const char *oldPath, const char *newPath) { return extFlashModule.rename(oldPath, newPath); }
} // namespace efc
#endif
