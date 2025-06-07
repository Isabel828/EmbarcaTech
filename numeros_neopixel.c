#include "numeros_neopixel.h"
#include "LabNeoPixel/neopixel_driver.h" // ajuste conforme o caminho real

static void mostrar_numero(const uint8_t indices[], uint tamanho, uint8_t r, uint8_t g, uint8_t b) {
    npClear();
    for (uint i = 0; i < tamanho; i++) {
        npSetLED(indices[i], r, g, b);
    }
    npWrite();
}

void mostrar_numero_1() {
    uint8_t indices[] = {22, 16, 17, 12, 7, 1, 2, 3};
    mostrar_numero(indices, sizeof(indices)/sizeof(indices[0]), COR_1_R, COR_1_G, COR_1_B);
}

void mostrar_numero_2() {
    uint8_t indices[] = { 21, 22, 23,  18, 13, 12, 11, 6, 1, 2, 3};
    mostrar_numero(indices, sizeof(indices)/sizeof(indices[0]), COR_2_R, COR_2_G, COR_2_B);
}

void mostrar_numero_3() {
    uint8_t indices[] = { 21, 22, 23, 18, 8,   13, 12, 11,  1, 2, 3};
    mostrar_numero(indices, sizeof(indices)/sizeof(indices[0]), COR_3_R, COR_3_G, COR_3_B);
}

void mostrar_numero_4() {
    uint8_t indices[] = {23, 21, 16, 18, 13, 12, 11, 10,8, 1 };
    mostrar_numero(indices, sizeof(indices)/sizeof(indices[0]), COR_4_R, COR_4_G, COR_4_B);
}

void mostrar_numero_5() {
    uint8_t indices[] = { 21, 22, 23, 16, 13, 12, 11, 8, 1, 2, 3};
    mostrar_numero(indices, sizeof(indices)/sizeof(indices[0]), COR_5_R, COR_5_G, COR_5_B);
}

void mostrar_numero_6() {
    uint8_t indices[] = { 21, 22, 23,  16, 13, 12, 11, 8,6, 1, 2, 3};
    mostrar_numero(indices, sizeof(indices)/sizeof(indices[0]), COR_6_R, COR_6_G, COR_6_B);
}
