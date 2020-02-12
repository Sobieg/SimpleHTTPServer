/*
    Copyright (c) Евгений Крамсаков.
    СПбПУ, ВШКиЗИ, 2020
*/

#ifndef HTTPSERV_ERRORS_H
#define HTTPSERV_ERRORS_H

#include <errno.h>
#include <stdio.h>

int sock_err(const char* function, int s);

#endif //HTTPSERV_ERRORS_H
