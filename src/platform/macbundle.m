#define __HFS_FORMAT__ 1

#import <Foundation/Foundation.h>

const char *getBundlePath(const char *suffix)
{
    NSString *bp = [[NSBundle mainBundle] resourcePath];
    NSString *append = [NSString stringWithUTF8String:suffix];
    bp = [[bp stringByAppendingString:@"/"] stringByAppendingString:append];

    return ((const char *)[bp cStringUsingEncoding:NSUTF8StringEncoding]);
}
