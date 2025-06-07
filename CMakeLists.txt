cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 1.5.1)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

# Define o tipo de placa
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Puxa o SDK do Pico
include(pico_sdk_import.cmake)

# Nome do projeto
project(NeoControlLab C CXX ASM)

# Inicializa SDK do Pico
pico_sdk_init()

# Define o executável e arquivos fonte
add_executable(NeoControlLab
    NeoControlLab.c
    testes_cores.c
    LabNeoPixel/util.c
    LabNeoPixel/neopixel_driver.c
    LabNeoPixel/efeitos.c
    efeito_curva_ar.c
    numeros_neopixel.c
)

# Gera cabeçalho a partir do arquivo PIO
pico_generate_pio_header(NeoControlLab ${CMAKE_CURRENT_LIST_DIR}/LabNeoPixel/ws2818b.pio)

# Nome e versão
pico_set_program_name(NeoControlLab "NeoControlLab")
pico_set_program_version(NeoControlLab "0.1")

# Habilita USB, desabilita UART
pico_enable_stdio_uart(NeoControlLab 0)
pico_enable_stdio_usb(NeoControlLab 1)

# Bibliotecas necessárias
target_link_libraries(NeoControlLab
    pico_stdlib
    hardware_pio
    hardware_clocks
    hardware_adc
    hardware_dma
    hardware_pwm
    hardware_gpio   # <-- Adicionado
    hardware_irq    # <-- Adicionado
)

# Diretórios com headers
target_include_directories(NeoControlLab PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/LabNeoPixel
)

# Gera arquivos .uf2 etc.
pico_add_extra_outputs(NeoControlLab)




