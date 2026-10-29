# Zephyr: una aplicación, muchas placas

Workspace de la charla **"Zephyr RTOS: una aplicación, muchas placas"**
(Congreso de Ingeniería Electrónica 2026, UTN FRA). Lo mantiene el
Laboratorio de Sistemas Embebidos.

La idea: **el mismo `main.c`, sin cambios**, compilado y grabado en placas de
Raspberry Pi, STMicroelectronics, Espressif, Nordic, NXP y PJRC, con núcleos
ARM, Xtensa y RISC-V. Las demos usan solo el hardware que ya trae cada placa
(LED, botón, acelerómetro, sensor de temperatura interno). Todo se hace con
`west` desde la consola.

## Contenido

```
cie2026-zephyr/
├── west.yml                  manifest: Zephyr v4.5.0 + solo los HALs necesarios
├── apps/
│   ├── blinky_boton/         LED + botón con interrupción (alias led0 / sw0)
│   ├── acelerometro/         lee el acelerómetro (alias accel0) y muestra lo que mida
│   │   ├── boards/           cada placa dice cuál es su bus I2C (sensor_i2c)
│   │   ├── mpu6050.overlay   agrega un MPU6050 externo y lo hace accel0
│   │   └── prj.conf          APIs + logging + shell
│   └── temperatura/          lee el sensor de temperatura interno (alias die-temp0)
│       └── boards/           habilita el sensor en cada SoC
└── docs/
    └── guia-asistentes.md    guía paso a paso
```

## Instalación

Requisitos: Linux, macOS o Windows, Python 3.10 o superior, Git, CMake,
Ninja y el compilador de devicetree (`dtc`). Los pasos de cada sistema están
en la [Getting Started Guide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html)
de Zephyr, sección *Install dependencies*. Después:

```bash
mkdir ~/zephyr-ws && cd ~/zephyr-ws
git clone https://github.com/utn-fra-lse/cie2026-zephyr

python3 -m venv .venv
source .venv/bin/activate          # Windows: .venv\Scripts\activate
pip install west

west init -l cie2026-zephyr        # usa nuestro west.yml como manifest
west update --narrow -o=--depth=1  # Zephyr v4.5.0 + HALs (~3 GB)
west zephyr-export
west packages pip --install
west sdk install -t arm-zephyr-eabi riscv64-zephyr-elf \
    xtensa-espressif_esp32_zephyr-elf xtensa-espressif_esp32s3_zephyr-elf
```

## Compilar

Desde `~/zephyr-ws`, con el entorno activado:

```bash
# App 1: LED + botón
west build -p -b nrf5340dk/nrf5340/cpuapp cie2026-zephyr/apps/blinky_boton
west build -p -b teensy41 cie2026-zephyr/apps/blinky_boton

# App 2: acelerómetro integrado de la placa (FRDM-K64F, FRDM-MCXN236, LPCXpresso55S69)
west build -p -b frdm_k64f cie2026-zephyr/apps/acelerometro

# App 2: MPU6050 externo
west build -p -b nrf5340dk/nrf5340/cpuapp cie2026-zephyr/apps/acelerometro \
    -- -DEXTRA_DTC_OVERLAY_FILE=mpu6050.overlay

# App 3: temperatura interna del chip
west build -p -b esp32c3_supermini cie2026-zephyr/apps/temperatura
```

`-p` (*pristine*) borra el build anterior; hace falta al cambiar de placa o de
overlay.

## Grabar y ver la consola

```bash
west flash                      # programador por defecto de la placa
west flash -r <runner>          # elegir otro (ver tabla)
```

Consola serie a 115200 baudios: `picocom -b 115200 /dev/ttyACM0` en Linux,
`screen /dev/tty.usbmodem* 115200` en macOS, o PuTTY en Windows.

## Placas y hardware integrado

| Placa | Target `west` | Núcleo | LED | Botón | Acelerómetro | Temp. interna |
|---|---|---|:-:|:-:|---|:-:|
| Raspberry Pi Pico | `rpi_pico` | Cortex-M0+ | ✓ | — | — | ✓ |
| Raspberry Pi Pico 2 | `rpi_pico2/rp2350a/m33_0` | Cortex-M33 | ✓ | — | — | ✓ |
| WeAct Black Pill F401 | `blackpill_f401cc` | Cortex-M4F | ✓ | ✓ | — | ✓ |
| DOIT ESP32 DevKit V1 | `doit_esp32_devkit_v1/esp32/procpu` | Xtensa LX6 | ✓ | ✓ | — | — |
| ESP32-C3 SuperMini | `esp32c3_supermini` | RISC-V | ✓ | ✓ | — | ✓ |
| ESP32-S3 DevKitC | `esp32s3_devkitc/esp32s3/procpu` | Xtensa LX7 | RGB¹ | ✓ | — | ✓ |
| nRF5340 DK | `nrf5340dk/nrf5340/cpuapp` | Cortex-M33 | ✓ | ✓ | — (MPU6050 externo) | —² |
| LPCXpresso55S69 | `lpcxpresso55s69/lpc55s69/cpu0` | Cortex-M33 | ✓ | ✓ | MMA8652FC | — |
| FRDM-MCXN236 | `frdm_mcxn236` | Cortex-M33 | ✓ | ✓ | FXLS8974 (+ temperatura) | ✓ |
| FRDM-K64F | `frdm_k64f` | Cortex-M4F | ✓ | ✓ | FXOS8700 (+ magnetómetro) | ✓ |
| Teensy 4.1 | `teensy41` | Cortex-M7 | ✓ | — | — | ✓ |

¹ El LED de la ESP32-S3 DevKitC es un RGB direccionable, no un GPIO simple:
la placa no define `led0` y no corre `blinky_boton`.
² El sensor de temperatura del nRF5340 pertenece al núcleo de red, no al de
aplicación.

`blinky_boton` funciona también sin botón: si la placa no define `sw0` (Pico,
Teensy), el LED parpadea a velocidad fija.

### Qué placas sirven para cada demo

Cada demo usa solo el hardware que la placa ya trae. La única excepción es el
MPU6050 del nRF5340 DK, que no tiene acelerómetro propio.

| Demo | Placas |
|---|---|
| `blinky_boton` | Black Pill F401, DOIT ESP32, ESP32-C3 SuperMini, nRF5340 DK, LPCXpresso55S69, FRDM-MCXN236, FRDM-K64F, Teensy 4.1 (sin botón) |
| `acelerometro` | FRDM-K64F (FXOS8700), FRDM-MCXN236 (FXLS8974), LPCXpresso55S69 (MMA8652FC), nRF5340 DK + MPU6050 |
| `temperatura` | Pico, Pico 2, Black Pill F401, ESP32-C3 SuperMini, ESP32-S3 DevKitC, FRDM-MCXN236, FRDM-K64F, Teensy 4.1 |

Las demás combinaciones también compilan (por ejemplo, `acelerometro` con un
MPU6050 en cualquier placa) y se informan en la nota técnica como referencia de
uso de memoria.

### Cómo se graba cada placa

| Placa | Comando | Notas |
|---|---|---|
| Pico / Pico 2 | — | BOOTSEL al conectar y copiar `build/zephyr/zephyr.uf2` a la unidad USB |
| Black Pill F401 | `west flash` | dfu-util: mantener BOOT0, pulsar NRST, soltar BOOT0 |
| DOIT ESP32, ESP32-C3, ESP32-S3 | `west flash` | esptool por USB |
| nRF5340 DK | `west flash` | nrfutil + J-Link (programas de Nordic y SEGGER) |
| LPCXpresso55S69, FRDM-MCXN236 | `west flash -r linkserver` o `-r pyocd` | LinkServer es de NXP |
| FRDM-K64F | `west flash -r pyocd` | |
| Teensy 4.1 | `west flash` | `teensy_loader_cli` de PJRC; pulsar el botón de programación |

Notas:
- **pyocd** necesita el *pack* del micro una sola vez:
  `pyocd pack install k64f lpc55s69 mcxn236`.
- **Consola por adaptador USB-serie (3,3 V)**: la Black Pill usa USART1 (PA9 TX /
  PA10 RX) y la Teensy 4.1 usa LPUART6 (pin 1 TX / pin 0 RX).
- **MPU6050** (SDA / SCL): nRF5340 DK P1.02 / P1.03, FRDM-MCXN236 P4_0 / P4_1,
  Teensy 4.1 pines 18 / 19. Las demás placas están en `apps/acelerometro/boards/`.

## Cómo está armada la portabilidad

1. **La aplicación** usa solo los alias estándar de Zephyr (`led0`, `sw0`,
   `accel0`, `die-temp0`) y las API genéricas (`gpio_*`, `sensor_*`). Las placas
   que traen ese hardware ya definen los alias en su propio devicetree.
2. **El overlay de la placa** (`boards/<placa>.overlay`, se aplica solo) habilita
   lo que la placa no trae activado (por ejemplo, el sensor de temperatura) y le
   pone la etiqueta `sensor_i2c` a su bus I2C.
3. **`mpu6050.overlay`** (se pide con `-DEXTRA_DTC_OVERLAY_FILE`) cuelga el
   MPU6050 de `&sensor_i2c` y lo hace `accel0`.

Con N placas y M sensores alcanzan **N + M** archivos, no N × M.

## Licencia

Apache-2.0, igual que Zephyr.
