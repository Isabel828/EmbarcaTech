#ifndef FUNCOES_NEOPIXEL_H
#define FUNCOES_NEOPIXEL_H

#include <stdint.h>
#include <stdlib.h>

// stub inline para inicializar o “gerador”
// (vazio, só pra satisfazer o compilador)
static inline void inicializar_aleatorio(void) {
    // opcionalmente: srand(...);
}

// stub inline para gerar um “número aleatório”
// retorna sempre min, mas evita link errors
static inline uint16_t numero_aleatorio(uint16_t min, uint16_t max) {
    if (min >= max) return min;
    return (uint16_t)(min + (rand() % (max - min + 1)));
}

#endif // FUNCOES_NEOPIXEL_H
