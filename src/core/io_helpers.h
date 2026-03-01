#ifndef CK_IO_HELPERS_H
#define CK_IO_HELPERS_H

#define CK_IO_EXPECT(ok, expr, expected) \
    do { (ok) = (ok) && ((expr) == (expected)); } while (0)

#endif
