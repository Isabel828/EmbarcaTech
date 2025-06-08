/**
 * @file main_auxiliar.c
 * @brief Funções auxiliares do núcleo 0 no projeto multicore com Raspberry Pi Pico W.
 */

#include "fila_circular.h"
#include "rgb_pwm_control.h"
#include "configura_geral.h"
#include "oled_utils.h"
#include "ssd1306_i2c.h"
#include "mqtt_lwip.h"
#include "lwip/ip_addr.h"
#include "pico/multicore.h"
#include <stdio.h>
#include "estado_mqtt.h"
#include "funcoes_neopixel.h"  // para inicializar e gerar números aleatórios

// Garantir que a semente seja inicializada uma única vez
static bool aleatorio_inicializado = false;

void espera_usb() {
    while (!stdio_usb_connected()) {
        sleep_ms(200);
    }
    printf("Conexão USB estabelecida!\n");
}

typedef struct {
    uint16_t r, g, b;
} CorRGB;

// Lista de cores fortes e visíveis
CorRGB cores_fortes[] = {
    {65535, 0, 0},      // Vermelho
    {0, 65535, 0},      // Verde
    {0, 0, 65535},      // Azul
    {65535, 65535, 0},  // Amarelo
    {65535, 0, 65535},  // Magenta (roxo)
    {0, 65535, 65535},  // Ciano
};

void tratar_mensagem(MensagemWiFi msg) {
    const char *descricao = "";

    // ======= RESPOSTA AO PING =======
    if (msg.tentativa == 0x9999) {
        if (!aleatorio_inicializado) {
            inicializar_aleatorio();
            aleatorio_inicializado = true;
        }

        // Sorteia índice de uma cor forte
        size_t num_cores = sizeof(cores_fortes) / sizeof(cores_fortes[0]);
        size_t idx = numero_aleatorio(0, num_cores - 1);

        uint16_t r = cores_fortes[idx].r;
        uint16_t g = cores_fortes[idx].g;
        uint16_t b = cores_fortes[idx].b;

        // Aplica cor forte e indica no console
        set_rgb_pwm(r, g, b);
        printf("[MQTT] PING recebido. Cor forte sorteada: (R=%u, G=%u, B=%u)\n", r, g, b);

        // Mostra no OLED para confirmação do PING
        ssd1306_draw_utf8_multiline(buffer_oled, 0, 32, "ACK do PING OK");
        render_on_display(buffer_oled, &area);

        // Mantém cor forte por 1 segundo no OLED
        sleep_ms(1000);

        // Mantém a cor forte após o PING
        return;
    }

    // ======= STATUS Wi-Fi padrão =======
    switch (msg.status) {
        case 0:
            descricao = "INICIALIZANDO";
            set_rgb_pwm(PWM_STEP, 0, 0);  // LED vermelho
            break;
        case 1:
            descricao = "CONECTADO";
            set_rgb_pwm(0, PWM_STEP, 0);  // LED verde
            break;
        case 2:
            descricao = "FALHA";
            set_rgb_pwm(0, 0, PWM_STEP);  // LED azul
            break;
        default:
            descricao = "DESCONHECIDO";
            set_rgb_pwm(PWM_STEP, PWM_STEP, PWM_STEP);  // LED branco
            break;
    }

    char linha_status[32];
    snprintf(linha_status, sizeof(linha_status), "Status do Wi-Fi : %s", descricao);

    ssd1306_draw_utf8_multiline(buffer_oled, 0, 0, linha_status);
    render_on_display(buffer_oled, &area);
    sleep_ms(3000);
    oled_clear(buffer_oled, &area);
    render_on_display(buffer_oled, &area);

    printf("[NÚCLEO 0] Status: %s\n", descricao);
}

void tratar_ip_binario(uint32_t ip_bin) {
    char ip_str[20];
    uint8_t ip[4];

    ip[0] = (ip_bin >> 24) & 0xFF;
    ip[1] = (ip_bin >> 16) & 0xFF;
    ip[2] = (ip_bin >> 8) & 0xFF;
    ip[3] = ip_bin & 0xFF;

    snprintf(ip_str, sizeof(ip_str), "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

    oled_clear(buffer_oled, &area);
    ssd1306_draw_utf8_string(buffer_oled, 0, 0, ip_str);
    render_on_display(buffer_oled, &area);

    printf("[NÚCLEO 0] Endereço IP: %s\n", ip_str);
    ultimo_ip_bin = ip_bin;
}

void exibir_status_mqtt(const char *texto) {
    ssd1306_draw_utf8_string(buffer_oled, 0, 16, "MQTT: ");
    ssd1306_draw_utf8_string(buffer_oled, 40, 16, texto);
    render_on_display(buffer_oled, &area);

    printf("[MQTT] %s\n", texto);
}
