/*
    Copyright (c) Евгений Крамсаков.
    СПбПУ, ВШКиЗИ, 2020
*/

#include "errors.h"


int sock_err(const char* function, int s) {
    int err = errno;
    printf("%s: socket error: %s", function, err);
    return s;
}
