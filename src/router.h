#ifndef ROUTER_H
#define ROUTER_H

#include "parser.h"
#include <stdio.h>
#include <string.h>

int handle_request(http_request *request, char *response, int response_size);

#endif