#include "arch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "bytecode.h"

void arch_print() {
    /*static int i = 0;

    int sum = 2;
    for (int i = 0; i < 9999999; i++) {
        sum += pow(sum, 2);
    }

    i++;*/
    printf("moins asfd\n");
}

void* call_addr(void* source, void* target) {
    if (source > target) {
        return (void*)0xFFFFFFFF - (source - target);
    } else {
        return (void*)(target - source);
    }
}

void write1(char data, char* buffer, long* index) {
    buffer[*index] = data;
    *index += 1;
}

void write4(void* data, char* buffer, long* index) {
    memcpy(&buffer[*index], data, 4);
    *index += 4; 
}

arch_native_t arch_compile(ctr_header_t header, FILE* input) {
    arch_native_t native;

    // jump to text segment
    long text_offset = CTR_HEADER_SIZE + header.symbol_segment_size + header.data_segment_size;
    fseek(input, text_offset, SEEK_SET);

    // alloc space
    size_t text_size = header.text_segment_size + header.symbol_segment_size / CTR_SYMBOL_SIZE * 8;
    native.text = malloc(text_size);

    long index = 0; //3;
    write1(0xC8, native.text, &index);
    write1(0x04, native.text, &index);
    write1(0x00, native.text, &index);
    write1(0x00, native.text, &index);
    /*unsigned int enter = swap_endian(0xC8040000);
    memcpy(ptr, &enter, 4);
    ptr+=4;*/


    // compile to x86 code
    char opcode[BC_OPCODE_SIZE];
    char* instruction = opcode;
    unsigned int* arg = (unsigned int*)&opcode[1];
    while (!feof(input)) {
        fread(opcode, sizeof(char), sizeof(opcode), input);
        switch(*instruction) {
            case BC_NOP: {
                write1(0x90, native.text, &index);
                break;
            }
            case BC_SYNC: {
                write1(0xE8, native.text, &index);
                void* addr = call_addr(native.text + index + 4, arch_print);
                write4(&addr, native.text, &index);
                break;
           }
           default: {
                write1(0x90, native.text, &index);
                break;
            }
        }
    }
    //write1(0xE8, native.text, &index);
    //void* addr = call_addr(native.text + index + 4, arch_print);
    //memcpy(&native.text[index], &addr, 4);
    //write4(&addr, native.text, &index);
    //index += 4;
 
    //write1(0x90, native.text, &index);
    write1(0xC9, native.text, &index);
    write1(0xC3, native.text, &index);

    char* text = native.text;
    for (int i = 0; i < 40; i+=4) {
        for (int j = 0; j < 4; j++) {
            unsigned int value = text[i+j];
            value = (value << 24) >> 24;
            printf("%02X ", value);
        }
        printf("\n");
    }

    /**ptr = 0xE8;
    ptr++;
    void* addr = call_addr(ptr+4, arch_print);
    memcpy(ptr, &addr, 4);
    ptr+=4;
    *ptr = 0x90;
    ptr++;
    *ptr = 0x90;
    ptr++;
    *ptr = 0x90;
    ptr++;

    unsigned int leave = swap_endian(0xC9C39090);
    memcpy(ptr, &leave, 4);
    ptr+=4;*/

    native.main = native.text;

    return native;
}
