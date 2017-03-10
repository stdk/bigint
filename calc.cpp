#include <cstdio>

#include "bigint.h"

void show_result(const bigint_t &value) {
    printf("%s\n",value.toString(10).c_str());
}

int main(int argc, char **argv) {
    if(argc < 4) {
        printf("Usage: %s {plus|minus|mul|div} arg1 arg2\n",argv[0]);
        return 1;
    }

    bigint_t arg1(argv[2],10);
    bigint_t arg2(argv[3],10);

    if(strcmp(argv[1],"plus") == 0) {
        show_result(arg1+arg2);
    } else if(strcmp(argv[1],"minus") == 0) {
        show_result(arg1-arg2);
    } else if(strcmp(argv[1],"mul") == 0) {
        show_result(arg1*arg2);
    } else if(strcmp(argv[1],"div") == 0) {
        show_result(arg1/arg2);
    } else {
        printf("Invalid operation[%s]. Available: {plus|minus|mul|div} \n",argv[1]);
        return 2;
    }

    return 0;
}
