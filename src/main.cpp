#include <cstdio>
#include "utils\array.h"

int main() {
    Array<int32_t> vary(4);
    vary.add(100);
    vary.add(20);
    vary.add(3);
    vary.add(4);
    vary.add(4);
    vary.append(5);
    vary[3] = 12;
    int32_t ary[4] = {1, 2, 3, 4};
    for (uint32_t i = 0; i < vary.getUsedSize(); ++i) {
        printf("vary [%d]:%d\n", i, vary[i]);
    }
    printf("varyCapacity:%d/%d\n", vary.getUsedSize(), vary.getCapacity());
    vary[16] = 0;
    return 0;
}
