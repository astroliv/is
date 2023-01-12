#include <cstdio>
#include "utils/array.h"
#include "utils/bytecode.h"


int main() {
    Array<uint8_t> vary(4);
    vary.add(100);
    vary.add(20);
    vary.add(3);
    vary.add(4);
    vary.add(4);
    vary.append(5);
    vary[3] = 12;
    int32_t ary[4] = {1, 2, 3, 4};
    writeArg(vary, 1212301123);
    printf("%llu\n", readArg(vary, 5));
    for (isize i = 0; i < vary.getUsedSize(); ++i) {
        printf("vary [%d]:%d\n", i, vary[i]);
    }
    printf("varyCapacity:%d/%d\n", vary.getUsedSize(), vary.getCapacity());
    vary[666] = 0;
    printf("A statement after index out of range.\n");
    vary[666] = 0;
    return 0;
}
