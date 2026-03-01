#ifndef CK_IO_HELPERS_H
#define CK_IO_HELPERS_H

#define CK_IO_EXPECT(ok, expr, expected) \
    do { (ok) = (ok) && ((expr) == (expected)); } while (0)

#define CK_IO_WARN_ON_WRITE_FAIL(ok, path) \
    do { \
        if (!(ok)) \
            CVort_engine_cross_logMessage(CVORT_LOG_MSG_WARNING, "Failed writing %s.\n", (path)); \
    } while (0)

#endif
