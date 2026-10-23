# Changes

External flash for OpenKNX devices: a LittleFS volume on an SPI flash chip, offered to the rest of the
firmware as a file-store provider. Entries are grouped by module version; every line traces to a commit
in this repository.


## 0.0.2: 2026-10-23

* Change: the `efc` console commands are built only with `OPENKNX_CONSOLE_DEV`; without it the module answers no `efc` command
* Fix: the module builds on a coupler mask without group objects -- it no longer overrides `processInputKo()`, which the base class declares only when group objects exist


## 0.0.1: 2026-09-15

First tagged state. The module has existed since 2024 as a store wired into the file transfer client,
and was reworked into a provider this year.

**File store**
* Change: the module is a pure provider -- it exposes `efc::IFileStore` and no longer reaches into the file transfer client, which registers the `efc/` backend itself
* Feature: positioned write and rename on `EfcFileStore`
* Feature: `isDir()` and a busy guard, so a second operation cannot start while one is running
* Feature: `sinkOpen` takes an offset, so an interrupted transfer resumes instead of starting over; the size hint is accepted and ignored, because LittleFS does not preallocate

**Console**
* Feature: `efc` actions -- test, info, format, add, rm, cat, echo, mv, cp, mkdir, rmdir, ls, ll
* Feature: a time callback for the external LittleFS, so files carry a real timestamp

**Build flags**
* Change: `EXTERNAL_FLASH_MODULE` is `OPENKNX_EXTFLASH`; the old name stays as an alias
* The module is behind an ifdef guard

**Hardware**
* Change: the SPI GPIO configuration moved into the hardware header
* ESP32 is not supported yet
