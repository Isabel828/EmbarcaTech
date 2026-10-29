# EmbarcaTech — Sistemas Embarcados

Repositório de exercícios e projetos práticos de programação em **C** e aplicações embarcadas desenvolvidos no contexto da formação EmbarcaTech.

## Conteúdo identificado

- Exercícios introdutórios em C: condicionais, estruturas de repetição, vetores e funções.
- Arquivos de aplicação com controle de LEDs **NeoPixel**, módulos de efeitos e testes de cores.
- Projeto com **Raspberry Pi Pico SDK**, **CMake**, referências a MQTT, Wi-Fi e periféricos.
- Atividades organizadas também nas pastas `M2/` e `M3/`.

## Tecnologias

C · CMake · Raspberry Pi Pico SDK · Git/GitHub. O `CMakeLists.txt` principal define `PICO_BOARD=pico_w` e um alvo chamado `MQTT_2`.

## Estrutura (resumo)

```text
EmbarcaTech/
├── 03-sec.c ... 12-func.c  # Exercícios em C
├── main.c                  # Aplicação embarcada
├── main_auxiliar.c
├── estado_mqtt.c/.h
├── NeoControlLab.c
├── numeros_neopixel.c/.h
├── CMakeLists.txt
├── pico_sdk_import.cmake
├── M2/
├── M3/
└── build/                  # Artefatos versionados, revisar antes de limpar
```

## Execução

Para os exercícios simples de C, a compilação depende do arquivo escolhido (por exemplo, `gcc 05-if.c -o exemplo` e `./exemplo`, se o arquivo tiver uma função `main` compatível).

Para o firmware do Pico W, instale e configure o **Raspberry Pi Pico SDK**, a cadeia de compilação para ARM e o CMake. **A compilação do projeto principal não foi validada nesta auditoria**: o `CMakeLists.txt` referencia caminhos `WIFI_/` e `OLED_/` que não aparecem na listagem da raiz; verifique essas dependências antes de executar o build.

## Melhorias futuras

Revisar caminhos de compilação, dividir exercícios por módulo e remover artefatos de build versionados **apenas após revisão e autorização**. Os arquivos originais foram preservados.

**Créditos:** atividades acadêmicas da formação EmbarcaTech.
