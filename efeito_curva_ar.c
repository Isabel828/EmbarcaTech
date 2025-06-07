#include <stdlib.h>             // rand() e RAND_MAX
#include "pico/stdlib.h"
#include "LabNeoPixel/neopixel_driver.h"
#include "efeito_curva_ar.h"

#define TAM 5
static float coef[TAM] = {0.4, -0.2, 0.15, 0.1, 0.05};
static float estados[TAM] = {0.0};
static int coluna_atual = 0;

extern npLED_t leds[LED_COUNT];  


// Gera ruído aleatório entre -amp e +amp
static float ruido_aleatorio(float amp) {
    return ((float)rand() / RAND_MAX) * 2 * amp - amp;
}

// Gera próximo valor da série AR(5)
static float proximo_valor_ar() {
    float valor = 0.0;
    for (int i = 0; i < TAM; i++) {
        valor += coef[i] * estados[i];
    }
    valor += ruido_aleatorio(1.0f);

    for (int i = TAM - 1; i > 0; i--) {
        estados[i] = estados[i - 1];
    }
    estados[0] = valor;

    return valor;
}

// Efeito gráfico com barras verticais, partindo da linha central (2)
void efeitoCurvaNeoPixel(uint8_t r, uint8_t g, uint8_t b, uint16_t delay_ms) {
    float valor = proximo_valor_ar();

    int linha_ref = 2;
    int deslocamento = (int)(valor * 1.5f);
    int linha_destino = linha_ref - deslocamento;

    if (linha_destino < 0) linha_destino = 0;
    if (linha_destino > NUM_LINHAS - 1) linha_destino = NUM_LINHAS - 1;

    // --- Etapa 1: desloca toda a matriz uma coluna à esquerda
    for (int linha = 0; linha < NUM_LINHAS; linha++) {
        for (int coluna = 0; coluna < NUM_COLUNAS - 1; coluna++) {
            int idx_atual = linha * NUM_COLUNAS + coluna;
            int idx_prox  = linha * NUM_COLUNAS + coluna + 1;
            leds[idx_atual] = leds[idx_prox];  // desloca o conteúdo da direita para a esquerda
        }
    }

    // --- Etapa 2: escreve nova barra na coluna final (coluna 4)
    int nova_coluna = NUM_COLUNAS - 1;
    int inicio = linha_ref;
    int fim = linha_destino;
    if (inicio > fim) {
        int temp = inicio;
        inicio = fim;
        fim = temp;
    }

    for (int linha = 0; linha < NUM_LINHAS; linha++) {
        int index = linha * NUM_COLUNAS + nova_coluna;
        if (linha >= inicio && linha <= fim) {
            npSetLED(index, r, g, b);
        } else {
            npSetLED(index, 0, 0, 0); // apaga o restante da coluna
        }
    }

    // Atualiza os LEDs
    npWrite();
    sleep_ms(delay_ms);
}
