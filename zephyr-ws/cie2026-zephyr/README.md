# Zephyr: una aplicación, muchas placas

Workspace de la charla **"Zephyr RTOS: una aplicación, muchas placas"**
(Congreso de Ingeniería Electrónica 2026, UTN FRA). Lo mantiene el
Laboratorio de Sistemas Embebidos.

La idea: **el mismo `main.c`, sin cambios**, compilado y grabado en placas de
Raspberry Pi, STMicroelectronics, Espressif, Nordic y NXP, con núcleos ARM,
Xtensa y RISC-V. Por placa solo cambia el `-b <placa>` y, cuando hace falta, un
*overlay* de devicetree de pocas líneas. Todo se hace con `west` desde la
consola.

## Contenido

```
cie2026-zephyr/
├── west.yml                  manifest: Zephyr v4.4.2 + solo los HALs necesarios
├── apps/
│   ├── blinky_boton/         LED + pulsador con interrupción (alias led0 / sw0)
│   │   └── boards/           overlays solo para las placas sin LED o sin botón
│   └── acelerometro/         lee "el sensor" (alias sensor0) y muestra lo que mida
│       ├── boards/           cada placa dice cuál es su bus I2C (sensor_i2c)
│       │                     y, si tiene, cuál es su acelerómetro integrado
│       ├── mpu6050.overlay   agrega un MPU6050 externo y lo hace sensor0
│       └── prj.conf          APIs + logging + shell
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
west update --narrow -o=--depth=1  # Zephyr v4.4.2 + HALs (~3 GB)
west zephyr-export
west packages pip --install
west sdk install -t arm-zephyr-eabi riscv64-zephyr-elf \
    xtensa-espressif_esp32_zephyr-elf xtensa-espressif_esp32s3_zephyr-elf
```

## Compilar

Desde `~/zephyr-ws`, con el entorno activado:

```bash
# App 1: LED + botón
west build -p -b rpi_pico cie2026-zephyr/apps/blinky_boton
west build -p -b esp32c3_supermini cie2026-zephyr/apps/blinky_boton

# App 2: acelerómetro integrado de la placa (FRDM-K64F, LPCXpresso55S69)
west build -p -b frdm_k64f cie2026-zephyr/apps/acelerometro

# App 2: MPU6050 externo (cualquier placa)
west build -p -b blackpill_f401cc cie2026-zephyr/apps/acelerometro \
    -- -DEXTRA_DTC_OVERLAY_FILE=mpu6050.overlay
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

## Placas

| Placa | Target `west` | Núcleo | Overlay en blinky | I2C SDA / SCL | Acelerómetro integrado | Grabación |
|---|---|---|---|---|---|---|
| Raspberry Pi Pico | `rpi_pico` | Cortex-M0+ | pulsador GP15 | GP4 / GP5 | — | UF2: BOOTSEL + copiar `build/zephyr/zephyr.uf2` |
| Raspberry Pi Pico 2 | `rpi_pico2/rp2350a/m33` | Cortex-M33 | pulsador GP15 | GP4 / GP5 | — | UF2: BOOTSEL + copiar `zephyr.uf2` |
| WeAct Black Pill F401 | `blackpill_f401cc` | Cortex-M4F | — | PB9 / PB8 | — | `west flash` (dfu-util: BOOT0 + NRST) |
| DOIT ESP32 DevKit V1 | `doit_esp32_devkit_v1/esp32/procpu` | Xtensa LX6 | — | GPIO21 / GPIO22 | — | `west flash` (esptool, USB) |
| ESP32-C3 SuperMini | `esp32c3_supermini` | RISC-V | — | GPIO9 / GPIO10 | — | `west flash` (esptool, USB) |
| ESP32-S3 DevKitC | `esp32s3_devkitc/esp32s3/procpu` | Xtensa LX7 | LED GPIO10 | GPIO1 / GPIO2 | — | `west flash` (esptool, USB) |
| nRF5340 DK | `nrf5340dk/nrf5340/cpuapp` | Cortex-M33 | — | P1.02 / P1.03 | — | `west flash` (nrfutil + J-Link) |
| LPCXpresso55S69 | `lpcxpresso55s69/lpc55s69/cpu0` | Cortex-M33 | — | FLEXCOMM4 | MMA8652FC | `west flash -r linkserver` o `-r pyocd` |
| FRDM-MCXN236 | `frdm_mcxn236` | Cortex-M33 | — | P4_0 / P4_1 | — | `west flash -r linkserver` o `-r pyocd` |
| FRDM-K64F | `frdm_k64f` | Cortex-M4F | — | PTE25 / PTE24 | FXOS8700 (+ magnetómetro) | `west flash -r pyocd` |

Notas:
- **pyocd** necesita el *pack* del micro una sola vez:
  `pyocd pack install k64f lpc55s69 mcxn236`.
- **linkserver** y **nrfutil/J-Link** son programas de NXP y SEGGER que se
  instalan aparte. Los dos tienen versiones para Linux, macOS y Windows.
- **Black Pill**: la consola de Zephyr está en USART1 (PA9 TX / PA10 RX). Hace
  falta un adaptador USB-serie de 3,3 V.
- **ESP32-C3 SuperMini**: el I2C usa GPIO9, que también es el botón BOOT. No lo
  presiones con el sensor conectado.
- **ESP32-S3 DevKitC**: el LED de la placa es RGB direccionable (WS2812), así
  que blinky usa un LED externo en GPIO10 con una resistencia de 330 Ω.

## Cómo está armada la portabilidad

1. **La aplicación** usa solo alias del devicetree (`led0`, `sw0`, `sensor0`)
   y las API genéricas (`gpio_*`, `sensor_*`).
2. **El overlay de la placa** (`boards/<placa>.overlay`, se aplica solo) agrega
   lo que la placa no trae, le pone la etiqueta `sensor_i2c` a su bus I2C y,
   si la placa tiene acelerómetro, lo apunta con `sensor0`.
3. **`mpu6050.overlay`** (se pide con `-DEXTRA_DTC_OVERLAY_FILE`) cuelga el
   MPU6050 de `&sensor_i2c` y lo hace `sensor0`.

Con N placas y M sensores alcanzan **N + M** archivos, no N × M.

## Licencia

Apache-2.0, igual que Zephyr.
