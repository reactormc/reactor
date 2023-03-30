#include "uuid.h"

#include <stdlib.h>
#include <stdio.h>

int uuid_to_string(uuid_t *uuid, char **out, uint8_t dashed) {
    static const char hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
    static const int groups[] = { 8, 4, 4, 4, 12 };

    int bit = 0;

    *out = calloc(37, sizeof(char));
    if (!*out) {
        perror("uuid_to_string - calloc");
        return 1;
    }

    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < groups[i]; j += 2) {
            uint8_t byte = uuid->bit[bit++];
            **out++ = hex[byte >> 4];
            **out++ = hex[byte & 0xf];
        }

        if (dashed == 1) {
            **out++ = '-';
        }
    }

    if (dashed == 1) {
        out--;
    }

    **out = '\0';

    return 0;
}

int uuid_to_dashed_string(uuid_t *uuid, char **out) {
    return uuid_to_string(uuid, out, 1);
}

int uuid_to_dashless_string(uuid_t *uuid, char** out) {
    return uuid_to_string(uuid, out, 0);
}