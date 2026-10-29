# Guía para asistentes: tu primera app Zephyr en varias placas

Esta guía reproduce la demo de la charla con una computadora y la consola.
Lleva entre 30 y 45 minutos, de los cuales la mayor parte es la descarga
inicial.

## 0. Qué necesitás

- Una computadora con Linux, macOS o Windows y unos 5 GB libres.
- Una o más placas de la tabla del [README](../README.md).
- Las demos usan el hardware que ya trae cada placa: LED, botón, acelerómetro y
  sensor de temperatura interno. La tabla del README dice qué tiene cada una.
- Opcional, para la app del acelerómetro en placas que no lo traen: un módulo
  **MPU6050**.

## 1. Instalación

1. Instalá las dependencias del sistema siguiendo la sección *Install
   dependencies* de la [Getting Started Guide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html)
   (CMake, Ninja, Python, Git, `dtc`).
2. Creá el workspace con nuestro manifest:

```bash
mkdir ~/zephyr-ws && cd ~/zephyr-ws
git clone https://github.com/utn-fra-lse/cie2026-zephyr

python3 -m venv .venv
source .venv/bin/activate          # Windows: .venv\Scripts\activate
pip install west

west init -l cie2026-zephyr
west update --narrow -o=--depth=1
west zephyr-export
west packages pip --install
west sdk install -t arm-zephyr-eabi riscv64-zephyr-elf \
    xtensa-espressif_esp32_zephyr-elf xtensa-espressif_esp32s3_zephyr-elf
```

A partir de acá, todos los comandos se ejecutan desde `~/zephyr-ws` con el
entorno activado (`source .venv/bin/activate`).

## 2. Primera compilación: sin hardware

```bash
west build -p -b nrf5340dk/nrf5340/cpuapp cie2026-zephyr/apps/blinky_boton
```

Al final vas a ver una tabla de memoria (`FLASH`, `RAM`). Ahora, **sin tocar
nada del código**:

```bash
west build -p -b esp32c3_supermini cie2026-zephyr/apps/blinky_boton          # RISC-V
west build -p -b doit_esp32_devkit_v1/esp32/procpu cie2026-zephyr/apps/blinky_boton  # Xtensa
west build -p -b frdm_k64f cie2026-zephyr/apps/blinky_boton
west build -p -b teensy41 cie2026-zephyr/apps/blinky_boton                   # sin botón
```

`-p` (*pristine*) borra el build anterior, necesario al cambiar de placa.

> **Para mirar:** `build/zephyr/zephyr.dts` es el devicetree final, con todas
> las capas aplicadas. Buscá `led0` y `sw0` en la sección `aliases`:
>
> ```bash
> grep -A6 "aliases {" build/zephyr/zephyr.dts
> ```

## 3. Grabar y ver la salida

Conectá la placa y ejecutá:

```bash
west flash
```

La tabla del README dice qué programador usa cada placa y cuándo hace falta
`-r <runner>`. La **Raspberry Pi Pico** no usa `west flash`: mantené apretado
BOOTSEL al conectarla y copiá `build/zephyr/zephyr.uf2` a la unidad `RPI-RP2`
que aparece.

Si la placa no tiene botón (Pico, Teensy 4.1), `blinky_boton` lo detecta al
compilar y el LED parpadea a velocidad fija.

Abrí la consola serie a 115200 baudios:

```bash
picocom -b 115200 /dev/ttyACM0             # Linux (salir: Ctrl-A Ctrl-X)
screen /dev/tty.usbmodem* 115200           # macOS (salir: Ctrl-A K)
```

En Windows, PuTTY con el puerto `COMx` que aparezca. Deberías ver:

```
[00:00:00.001,000] <inf> blinky_boton: blinky_boton en nrf5340dk/nrf5340/cpuapp (SoC nrf5340)
[00:00:00.001,000] <inf> blinky_boton: Boton: gpio@842500 pin 23
[00:00:00.001,000] <inf> blinky_boton: LED  : gpio@842500 pin 28
[00:00:03.512,000] <inf> blinky_boton: Boton: periodo = 250 ms
```

## 4. App del acelerómetro: integrado vs. MPU6050

### Con el acelerómetro de la placa

En la FRDM-K64F, la FRDM-MCXN236 o la LPCXpresso55S69 no hace falta cablear nada:

```bash
west build -p -b frdm_k64f cie2026-zephyr/apps/acelerometro
west flash -r pyocd
```

La K64F trae un **FXOS8700**, que mide aceleración **y campo magnético** (valores de ejemplo):

```
[00:00:00.002,000] <inf> acelerometro: acelerometro en frdm_k64f/mk64f12 (SoC mk64f12)
[00:00:00.002,000] <inf> acelerometro: Sensor: fxos8700@1d
[00:00:00.004,000] <inf> acelerometro: Aceleracion  x=   0.123 y=  -0.087 z=   9.794 m/s2
[00:00:00.004,000] <inf> acelerometro: Campo magn.  x=   0.215 y=   0.031 z=  -0.402 G
```

En la LPCXpresso55S69 (MMA8652FC) vas a ver **solo aceleración**: es otro
chip, pero lo atiende el mismo driver, y la placa lo configura en modo solo
acelerómetro. En la FRDM-MCXN236, el **FXLS8974** informa aceleración y
**temperatura**.

Estas placas ya definen el alias estándar `accel0` en su devicetree: la app lo
usa directamente, sin overlay.

### Con un MPU6050 externo

Cableá el MPU6050 al bus I2C de tu placa (SDA, SCL, 3V3 y GND; ver la tabla
del README) y agregá su overlay:

```bash
west build -p -b nrf5340dk/nrf5340/cpuapp cie2026-zephyr/apps/acelerometro \
    -- -DEXTRA_DTC_OVERLAY_FILE=mpu6050.overlay
west flash
```

Mismo `main.c`, pero ahora aparecen **giro** y **temperatura del chip**, y no
hay campo magnético (valores de ejemplo):

```
[00:00:00.003,000] <inf> acelerometro: acelerometro en nrf5340dk/nrf5340/cpuapp (SoC nrf5340)
[00:00:00.003,000] <inf> acelerometro: Sensor: mpu6050@68
[00:00:00.005,000] <inf> acelerometro: Aceleracion  x=   0.311 y=   0.054 z=   9.702 m/s2
[00:00:00.005,000] <inf> acelerometro: Giro         x=  -0.012 y=   0.004 z=   0.001 rad/s
[00:00:00.005,000] <inf> acelerometro: Temp. chip     27.412 C
```

> Probá también el MPU6050 **en la FRDM-K64F**: el overlay externo reemplaza
> al sensor integrado sin tocar el código.

Si sale `El sensor mpu6050@68 no respondio`: revisá el cableado y la
dirección. Con AD0 a VCC la dirección es `0x69`; cambiala en
`apps/acelerometro/mpu6050.overlay` (`mpu6050@69` y `reg = <0x69>`).

Si compilás sin el overlay en una placa sin acelerómetro integrado, el error
de compilación te lo recuerda:

```
error: #error "Falta el alias 'accel0': esta placa no tiene acelerometro integrado. ..."
```

## 5. App de temperatura: el sensor interno del chip

Casi todos los microcontroladores traen un sensor de temperatura. Cada fabricante
lo implementa distinto: el RP2040, el STM32, el Kinetis y el MCX lo leen por un
canal de su propio ADC; los ESP32-C3/S3 y el i.MX RT de la Teensy tienen un
periférico dedicado. Zephyr los expone a todos con el mismo alias, `die-temp0`, y
el mismo canal, `SENSOR_CHAN_DIE_TEMP`.

```bash
west build -p -b esp32c3_supermini cie2026-zephyr/apps/temperatura
west flash
```

```
[00:00:00.002,000] <inf> temperatura: temperatura en esp32c3_supermini/esp32c3 (SoC esp32c3)
[00:00:00.002,000] <inf> temperatura: Sensor: coretemp@60040058
[00:00:00.004,000] <inf> temperatura: Temperatura del chip: 31.2 C
```

Funciona en Pico, Pico 2, Black Pill F401, ESP32-C3, ESP32-S3, FRDM-MCXN236,
FRDM-K64F y Teensy 4.1. Mirá los overlays de `apps/temperatura/boards/`: en
algunas placas alcanza con habilitar un nodo; en la MCXN236 además hay que
configurar el canal del ADC. Es la temperatura **del chip**, no del ambiente:
suele estar unos grados por encima.

## 6. Explorar el hardware con la shell

La app del acelerómetro trae la shell de Zephyr activada (ver `prj.conf`).
Las lecturas se imprimen cada 2 segundos; entre medio podés escribir comandos
en la consola:

```
uart:~$ device list
uart:~$ i2c scan i2c@9000              # nRF5340 DK (en otra placa, usá el nombre de device list)
uart:~$ sensor get mpu6050@68
uart:~$ kernel thread list
```

## 7. Desafíos

1. **Tu propia placa.** Elegí una placa que no esté en la tabla
   (`west boards | grep <algo>`) y escribí su `boards/<placa>.overlay` para la
   app del acelerómetro. Pista: buscá en su `.dts` un nodo `i2c` y ponele la
   etiqueta `sensor_i2c`.
2. **Dos sensores a la vez.** En la FRDM-K64F, agregá el MPU6050 como
   `accel1` sin pisar `accel0` y hacé que la app muestre los dos.
3. **Otro acelerómetro.** Zephyr tiene drivers para ADXL345, LIS3DH, BMI160 y
   muchos más. Escribí un `adxl345.overlay` (compatible `"adi,adxl345"`) y
   compilá con `-DEXTRA_DTC_OVERLAY_FILE=adxl345.overlay`.
4. **Kconfig por placa.** Creá `apps/acelerometro/boards/frdm_k64f.conf` con la
   línea `CONFIG_FXOS8700_MODE_ACCEL=y` y volvé a compilar para la FRDM-K64F.
   ¿Qué deja de mostrar la app? ¿Tuviste que cambiar `main.c`?
