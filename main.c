/**
 * @file main.c
 * @brief Núcleo 0 - Controle principal do sistema embarcado com Raspberry Pi Pico W.
 *
 * Este código é executado no núcleo 0 do RP2040 e desempenha as seguintes funções:
 * - Inicialização da interface OLED;
 * - Inicialização do PWM para LED RGB;
 * - Comunicação com o núcleo 1 (Wi-Fi);
 * - Tratamento das mensagens de status da conexão Wi-Fi;
 * - Inicialização do cliente MQTT após obtenção de IP;
 * - Envio periódico de "PING" via MQTT.
 */

#include "fila_circular.h"     // Fila circular para armazenar mensagens do núcleo 1
#include "rgb_pwm_control.h"   // Controle do LED RGB via PWM
#include "configura_geral.h"   // Configurações gerais do sistema
#include "oled_utils.h"        // Funções auxiliares para OLED
#include "ssd1306_i2c.h"       // Driver do display OLED via I2C
#include "mqtt_lwip.h"         // Cliente MQTT baseado em LWIP
#include "lwip/ip_addr.h"      // Manipulação de endereços IP
#include "pico/multicore.h"    // Comunicação entre núcleos do RP2040
#include <stdio.h>
#include "estado_mqtt.h"       // Variável global do estado do MQTT

// Intervalo entre envios de "PING" via MQTT
#define INTERVALO_PING_MS 5000

// Protótipos de funções externas e internas
extern void funcao_wifi_nucleo1(void);
extern void espera_usb();
extern void tratar_ip_binario(uint32_t ip_bin);
extern void tratar_mensagem(MensagemWiFi msg);
void inicia_hardware();
void inicia_core1();
void verificar_fifo(void);
void tratar_fila(void);
void inicializar_mqtt_se_preciso(void);
void enviar_ping_periodico(void);

// Fila circular para mensagens Wi-Fi
FilaCircular fila_wifi;
// Tempo para o próximo envio de PING
absolute_time_t proximo_envio;
// Buffer de mensagem para display ou debug
char mensagem_str[50];
// Flag de verificação de recebimento de IP
bool ip_recebido = false;

int main() {
    inicia_hardware();   // Inicializa periféricos (OLED, USB)
    inicia_core1();      // Inicia núcleo 1 responsável pelo Wi-Fi

    while (true) {
        verificar_fifo();           // Verifica se há novas mensagens do núcleo 1
        tratar_fila();             // Processa mensagens na fila circular
        inicializar_mqtt_se_preciso();  // Inicia cliente MQTT se IP recebido
        enviar_ping_periodico();   // Envia "PING" periodicamente via MQTT
        sleep_ms(50);              // Aguarda 50 ms antes de repetir o loop
    }

    return 0;
}

/*******************************************************************/

// Verifica FIFO entre os núcleos e insere na fila se houver mensagem válida
void verificar_fifo(void) {
    if (!multicore_fifo_rvalid()) return;  // Nada a ler no FIFO

    uint32_t pacote = multicore_fifo_pop_blocking();  // Lê pacote do núcleo 1
    uint16_t tentativa = pacote >> 16;                // Extrai número da tentativa
    uint16_t status = pacote & 0xFFFF;                // Extrai status da conexão

    // Código especial indicando que IP foi recebido
    if (tentativa == 0xFFFE) {
        uint32_t ip_bin = multicore_fifo_pop_blocking();
        tratar_ip_binario(ip_bin);  // Converte e armazena IP
        ip_recebido = true;
        return;
    }

    // Verificação de status inválido (proteção)
    if (status > 2 && tentativa != 0x9999) {
        snprintf(mensagem_str, sizeof(mensagem_str),
                 "Status inválido: %u (tentativa %u)", status, tentativa);
        ssd1306_draw_utf8_multiline(buffer_oled, 0, 0, "Status inválido.");
        render_on_display(buffer_oled, &area);
        sleep_ms(3000);
        oled_clear(buffer_oled, &area);
        render_on_display(buffer_oled, &area);
        printf("%s\n", mensagem_str);
        return;
    }

    // Insere mensagem na fila circular
    MensagemWiFi msg = {.tentativa = tentativa, .status = status};
    if (!fila_inserir(&fila_wifi, msg)) {
        ssd1306_draw_utf8_multiline(buffer_oled, 0, 0, "Fila cheia. Descartado.");
        render_on_display(buffer_oled, &area);
        sleep_ms(3000);
        oled_clear(buffer_oled, &area);
        render_on_display(buffer_oled, &area);
        printf("Fila cheia. Mensagem descartada.\n");
    }
}

// Trata mensagens da fila circular uma por vez
void tratar_fila(void) {
    MensagemWiFi msg_recebida;
    if (fila_remover(&fila_wifi, &msg_recebida)) {
        tratar_mensagem(msg_recebida);  // Aplica ação conforme status/tentativa
    }
}

// Inicia cliente MQTT se IP válido foi recebido e ainda não iniciou
void inicializar_mqtt_se_preciso(void) {
    if (!mqtt_iniciado && ultimo_ip_bin != 0) {
        printf("[MQTT] Iniciando cliente MQTT...\n");
        iniciar_mqtt_cliente();  // Função definida em mqtt_lwip
        mqtt_iniciado = true;
        proximo_envio = make_timeout_time_ms(INTERVALO_PING_MS);
    }
}

// Envia "PING" via MQTT a cada intervalo definido
void enviar_ping_periodico(void) {
    if (mqtt_iniciado && absolute_time_diff_us(get_absolute_time(), proximo_envio) <= 0) {
        publicar_mensagem_mqtt("PING");  // Envia tópico MQTT
        printf("[MQTT] PING publicado\n");

        // Exibe no OLED
        ssd1306_draw_utf8_multiline(buffer_oled, 0, 0, "PING enviado...");
        render_on_display(buffer_oled, &area);

        // Define próximo tempo de envio
        proximo_envio = make_timeout_time_ms(INTERVALO_PING_MS);
    }
}

// Inicializa periféricos básicos: serial, OLED e aguarda USB
void inicia_hardware() {
    stdio_init_all();       // Inicia comunicação serial
    setup_init_oled();      // Inicia display OLED
    espera_usb();           // Aguarda conexão USB (debug)
    oled_clear(buffer_oled, &area);
    render_on_display(buffer_oled, &area);
}

// Inicializa núcleo 1 e estruturas de controle
void inicia_core1() {
    // Mensagem no OLED
    ssd1306_draw_utf8_multiline(buffer_oled, 0, 0, "Núcleo 0");
    ssd1306_draw_utf8_multiline(buffer_oled, 0, 16, "Iniciando!");
    render_on_display(buffer_oled, &area);
    sleep_ms(3000);
    oled_clear(buffer_oled, &area);
    render_on_display(buffer_oled, &area);

    // Mensagem via USB serial
    printf(">> Núcleo 0 iniciado. Aguardando mensagens do núcleo 1...\n");

    // Inicializações
    init_rgb_pwm();                 // Prepara controle PWM do LED RGB
    fila_inicializar(&fila_wifi);  // Inicializa fila circular
    multicore_launch_core1(funcao_wifi_nucleo1);  // Inicia o núcleo 1
}
