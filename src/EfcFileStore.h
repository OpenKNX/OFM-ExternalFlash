// Backward-compat: flag renamed EXTERNAL_FLASH_MODULE -> OPENKNX_EXTFLASH; old name still works.
#if defined(EXTERNAL_FLASH_MODULE) && !defined(OPENKNX_EXTFLASH)
    #define OPENKNX_EXTFLASH
#endif
#pragma once
// efc::IFileStore — external-flash file store; identical API to sd::IFileStore, no shared base. FS-free header.
#ifdef OPENKNX_EXTFLASH
    #include <cstdint>

namespace efc
{
    class IFileStore
    {
      public:
        bool available();
        uint64_t totalBytes();
        uint64_t freeBytes();
        int32_t open(const char *path);
        uint8_t read(uint32_t offset, uint8_t *buf, uint8_t len);
        void close();
        bool exists(const char *path);
        bool sinkOpen(const char *path, uint32_t offset = 0);
        int sinkWrite(const uint8_t *buf, uint16_t len);
        void sinkClose();
        bool dirOpen(const char *path);
        uint8_t dirNext(char *nameOut, uint16_t cap, uint32_t *sizeOut = nullptr); // 0 end · 1 file · 2 dir
        void dirClose();
        bool remove(const char *path);
        bool mkdir(const char *path);
        bool rmdir(const char *path);
    };

    extern IFileStore fileStore;
} // namespace efc
#endif
