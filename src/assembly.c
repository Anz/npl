#include "assembly.h"
#include <stdlib.h>
#include <string.h>
#include "map.h"

map_t assembly;

void asm_init() {
    char* mnemonics[] = {
        "arg",
        "init",
        "call",
        "cmp"
    };

    map_init(&assembly, MAP_STR, sizeof(char));
    for (char i = 0; i < sizeof(mnemonics) / sizeof(char*); i++) {
        map_set(&assembly, mnemonics[(int)i], &i);
    }
}

void asm_release() {
    map_release(&assembly);
}

char asm_mnemonic2opcode(char* mnemonic) {
    char* opcode = map_find_key(&assembly, mnemonic);
    if (!opcode) {
        return -1;
    }

    return *opcode;
}

char* asm_opcode2mnemonic(char opcode) {
    return map_find_value(&assembly, &opcode);
}
