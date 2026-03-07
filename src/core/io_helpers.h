// io_helpers.h: Declares io helpers interfaces for the core subsystem.

#ifndef CK_IO_HELPERS_H
#define CK_IO_HELPERS_H

#define CK_IO_EXPECT(ok, expr, expected) \
    do { (ok) = (ok) && ((expr) == (expected)); } while (0)

#define CK_IO_WARN_ON_WRITE_FAIL(ok, path) \
    do { \
        if (!(ok)) \
            CVort_engine_cross_logMessage(CVORT_LOG_MSG_WARNING, "Failed writing %s.\n", (path)); \
    } while (0)

#define CK_IO_WARN_ON_READ_FAIL(ok, path) \
    do { \
        if (!(ok)) \
            CVort_engine_cross_logMessage(CVORT_LOG_MSG_WARNING, "Failed reading %s.\n", (path)); \
    } while (0)

#define CK_IO_RETURN_IF_OPEN_FAIL_FOR_WRITE(fp, path) \
    do { \
        if (!(fp)) { \
            CVort_engine_cross_logMessage(CVORT_LOG_MSG_WARNING, "Failed opening %s for writing.\n", (path)); \
            return; \
        } \
    } while (0)

#define CK_IO_WARN_ON_OPEN_FAIL_FOR_READ(path) \
    do { \
        CVort_engine_cross_logMessage(CVORT_LOG_MSG_WARNING, "Failed opening %s for reading.\n", (path)); \
    } while (0)

#endif
