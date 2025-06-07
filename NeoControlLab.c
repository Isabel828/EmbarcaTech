#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "neopixel_driver.h"
#include "testes_cores.h"
#include "efeitos.h"
#include "efeito_curva_ar.h"
#include "numeros_neopixel.h"
#include <stdlib.h>
#include "pico/time.h"

#define BOTAO_A 5

volatile bool botao_pressionado = false;

// Callback da interrupção do botão
void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == BOTAO_A && (events & GPIO_IRQ_EDGE_FALL)) {
        botao_pressionado = true;
    }
}

int sorteia_entre(int min, int max) {
    return rand() % (max - min + 1) + min;
}

void mostrar_numero_sorteado(int numero) {
    npClear();
    switch (numero) {
        case 1: mostrar_numero_1(); break;
        case 2: mostrar_numero_2(); break;
        case 3: mostrar_numero_3(); break;
        case 4: mostrar_numero_4(); break;
        case 5: mostrar_numero_5(); break;
        case 6: mostrar_numero_6(); break;
    }
}

void configurar_botao() {
    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_pull_up(BOTAO_A);
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
}

int main() {
    stdio_init_all();
    sleep_ms(1000);

    npInit(LED_PIN);
    npSetAll(100, 0, 0); // vermelho - inicialização
    npWrite();
    sleep_ms(500);
    npClear();
    npWrite();

    srand(time_us_32());

    configurar_botao();

    printf("Sistema pronto. Aguardando botão A (GPIO 14)...\n");

    while (true) {
        if (botao_pressionado) {
            botao_pressionado = false;

            printf("Botão A pressionado! Sorteando...\n");
            int vezes = sorteia_entre(10, 30);

            for (int i = 0; i < vezes; i++) {
                int n = sorteia_entre(1, 6);
                printf("Número sorteado: %d\n", n);
                mostrar_numero_sorteado(n);
                sleep_ms(300);
            }

            printf("Aguardando nova pressão do botão...\n");
        }

        sleep_ms(10);
    }
}



