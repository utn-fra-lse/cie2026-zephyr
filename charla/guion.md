# Guion: "Zephyr RTOS: una aplicación, muchas placas"

CIE 2026 · UTN FRA · 60 minutos · Fabrizio Carlassara (LSE)

**Mensaje único que se tiene que llevar el público:**
*"En Zephyr el hardware se describe, no se programa. Cambiar de placa, o de
sensor, es cambiar un archivo de descripción, no la aplicación."*

---

## Tiempos

| Min | Bloque | Desde → hasta | Margen |
|---:|---|---|---|
| 0–5 | Apertura y el problema | Portada → "¿Y si la aplicación no supiera…?" | — |
| 5–12 | Zephyr en cinco minutos | "¿Qué es Zephyr?" → "Zephyr vs. kernel solo" | se puede comprimir a 5 min |
| 12–24 | Devicetree + Kconfig | "Describir, no programar" → "Kconfig" | **núcleo, no recortar** |
| 24–31 | Modelo de drivers | "struct device" → "APIs estandarizadas" | se puede saltar la última |
| 31–48 | **Demo en vivo** | "El banco de pruebas" → "Resultados" | 17 min, con plan B |
| 48–53 | Herramientas, límites, cierre | "west" → "Probalo vos" | — |
| 53–60 | Preguntas | "¿Preguntas?" (volver al QR) | colchón real |

Si vas atrasado al minuto 31: saltear "APIs estandarizadas".
Si vas atrasado al 48: ir directo a "Para llevarse".

---

## Bloque 1 · Apertura y problema (0–5)

**Portada.** Presentarse en una línea y presentar el LSE. Pregunta a la sala:
*"¿Quién programó alguna vez un micro con el SDK del fabricante? ¿Y quién tuvo
que pasar ese proyecto a otro micro?"* (manos arriba).

**Hoja de ruta.** 20 segundos. Anticipar que hay demo en vivo con placas de seis
fabricantes y tres arquitecturas.

**Prender un LED, cuatro fabricantes.** Dejar que lean. Remarcar que no es solo
la sintaxis: el pin y el puerto están escritos en el código, y además cambian el
build, el IDE y la configuración de relojes.

**El costo de casarse con un fabricante.** Contar una anécdota propia (faltante
de chips, cambio de placa en un proyecto de cátedra o de la industria). El
diagrama muestra que la lógica de la app y la HAL quedan mezcladas.

**Standout.** Pausa de dos segundos. *"Eso es lo que vamos a ver hoy."*

## Bloque 2 · Zephyr en cinco minutos (5–12)

**¿Qué es Zephyr?** Las cifras salen del árbol de v4.5.0: más de 1100 placas,
185 fabricantes y 13 arquitecturas. Remarcar que lo impulsan los propios
fabricantes de silicio, que compiten entre sí pero comparten el RTOS.

**Arquitectura.** Recorrer de abajo hacia arriba. Las dos cajas amarillas son el
corazón de la charla: devicetree (hardware) y Kconfig (software), las dos
**resueltas al compilar**.

**Zephyr vs. "solo kernel".** No es una competencia. FreeRTOS es excelente en lo
suyo; Zephyr tiene otro alcance. Mencionar el costo: más memoria y más
formación inicial.

## Bloque 3 · Devicetree + Kconfig (12–24)

**Describir, no programar.** Origen en Linux. Diferencia clave: en Zephyr no hay
parser en el micro, todo se vuelve macros. *"Si te equivocás de pin, te enterás
al compilar, no con el osciloscopio."*

**Anatomía de un nodo.** Es el MPU6050 de la demo. Señalar en orden: label,
`@68`, `compatible`, `reg`, `aliases`. **`compatible` es el puente al driver.**
Los alias son el contrato entre la app y la placa.

**Capas.** SoC (fabricante) → placa → overlay (nosotros) → `zephyr.dts`. *"La
app nunca toca las dos primeras."*

**Bindings.** El YAML valida. Mencionar que hay más de 4400.

**Del árbol al código C.** Pipeline. Leer el comentario del código en voz alta:
la Pico resuelve `led0` a `gpio0`, pin 25, sin tablas en RAM.

**Kconfig.** Devicetree = qué hardware, Kconfig = qué software. Remarcar que **los
drivers se activan solos** por el devicetree y que hay un único `prj.conf` para
todas las placas. Anticipar que la shell ya está ahí.

## Bloque 4 · Modelo de drivers (24–31)

**`struct device`.** Config (flash) + data (RAM) + api (vtable). Una instancia por
nodo, creada al compilar. Se inicializan solas antes de `main`.

**Una API, muchas implementaciones.** Es polimorfismo en C: la vtable. *"El
devicetree decide qué función se ejecuta."*

**API de sensores.** Canales en unidades SI y `-ENOTSUP` para lo que el sensor no
mide. **Esto es lo que permite la app 2.** Mencionar de pasada la API asíncrona
`sensor_read()`.

**APIs estandarizadas** *(salteable)*. Mismo patrón en todas.

## Bloque 5 · Demo (31–48)

**El banco de pruebas.** Mostrar físicamente las placas sobre la mesa. La regla de
la demo: **solo se usa el hardware que ya trae cada placa** (LED, botón,
acelerómetro, sensor de temperatura). Remarcar los ESP32: el C3 es **RISC-V** y
el DOIT es **Xtensa**. Ninguno es ARM.

**App 1, `main.c`.** *"Ni un número de pin."* Señalar el `#if
DT_NODE_EXISTS(DT_ALIAS(sw0))`: si la placa no tiene botón (Teensy), la app lo
sabe al compilar y parpadea a velocidad fija.

**Lo que cambia por placa.** En blinky, nada: todas las placas de la demo ya
definen `led0` (y casi todas `sw0`) en su `.dts`.

**App 2, `main.c`.** La app ni siquiera sabe qué sensor tiene: pide el alias
estándar `accel0`, pregunta por aceleración, giro, campo magnético y
temperatura, y muestra lo que el driver responda. Las placas con acelerómetro ya
definen `accel0` en su `.dts`: no hace falta overlay.

**Placa + sensor.** N + M, no N × M. El overlay del MPU6050 se aplica después del
de la placa; en el nRF5340 DK, que no trae acelerómetro, es lo que lo agrega.

**Mismo `main.c`, distinto sensor.** La tabla anticipa lo que se va a ver en vivo.
Remarcar que FXOS8700 y MMA8652FC son dos chips con **un solo driver**, y que el
FXLS8974 de la MCXN236 recién aparece en el devicetree de la placa con Zephyr 4.5:
el soporte crece en cada versión.

**App 3, temperatura.** El ejemplo más claro de "describir, no programar": cada
fabricante mide la temperatura del chip de otra forma (canal del ADC con
calibración de fábrica, periférico dedicado), pero la app siempre pide
`die-temp0` y `SENSOR_CHAN_DIE_TEMP`. Mostrar la diferencia entre el overlay de
la ESP32-C3 (habilitar un nodo) y el de la MCXN236 (configurar el canal del ADC).

**Standout "Demo en vivo"** → cambiar a la terminal (ver procedimiento abajo).

**Resultados.** Volver a las diapositivas. Las placas que no se grabaron también
compilan: la tabla muestra el uso de memoria de todas, a modo de referencia.

**La shell.** Si la demo vino bien de tiempo, ya se usó en vivo. Si no, mostrar la
diapositiva y seguir.

## Bloque 6 · Herramientas, límites y cierre (48–53)

**west.** Un solo comando de flash para programadores de cinco fabricantes.

**Lo que Zephyr no resuelve.** Importante para la credibilidad. *"Portar no es
validar."*

**Para llevarse.** Cuatro puntos. Leerlos.

**Probalo vos.** Dejar el QR en pantalla durante las preguntas.

**¿Preguntas?** Volver a "Probalo vos" (QR) mientras se responde.

---

## Procedimiento de la demo (17 min)

### Preparación (la noche anterior)

- [ ] Workspace actualizado en la notebook (`west update`) y compilaciones
      verificadas.
- [ ] **Builds hechos de antemano** en directorios separados (`-d`), para que en
      vivo solo se compile **uno** (el nRF5340 DK, unos 20 s).
- [ ] Cada placa grabada con `blinky_boton` y funcionando (punto de partida conocido).
- [ ] MPU6050 cableado y probado en el nRF5340 DK (SDA P1.02, SCL P1.03).
- [ ] Adaptador USB-serie conectado a la Teensy 4.1 (pines 0 y 1) para ver su consola.
- [ ] `teensy_loader_cli`, LinkServer/pyocd, nrfutil y J-Link instalados y probados.
- [ ] Hub USB con alimentación externa. Rotular cada cable con el nombre de la placa.
- [ ] Terminal con dos paneles (o dos ventanas): comandos arriba y consola serie abajo.
- [ ] Fuente grande en la terminal (≥ 20 pt), tema claro para el proyector.
- [ ] Video de respaldo grabado (plan B).

Builds de antemano (desde `~/zephyr-ws`):

```bash
# blinky_boton
west build -p -b frdm_k64f -d build/blinky-k64f cie2026-zephyr/apps/blinky_boton
west build -p -b esp32c3_supermini -d build/blinky-c3 cie2026-zephyr/apps/blinky_boton
west build -p -b doit_esp32_devkit_v1/esp32/procpu -d build/blinky-doit cie2026-zephyr/apps/blinky_boton
west build -p -b teensy41 -d build/blinky-teensy cie2026-zephyr/apps/blinky_boton

# acelerometro
west build -p -b frdm_k64f -d build/acel-k64f cie2026-zephyr/apps/acelerometro
west build -p -b frdm_mcxn236 -d build/acel-mcxn cie2026-zephyr/apps/acelerometro
west build -p -b nrf5340dk/nrf5340/cpuapp -d build/acel-nrf cie2026-zephyr/apps/acelerometro -- -DEXTRA_DTC_OVERLAY_FILE=mpu6050.overlay

# temperatura
west build -p -b esp32c3_supermini -d build/temp-c3 cie2026-zephyr/apps/temperatura
west build -p -b teensy41 -d build/temp-teensy cie2026-zephyr/apps/temperatura
west build -p -b frdm_mcxn236 -d build/temp-mcxn cie2026-zephyr/apps/temperatura
```

### En vivo

| # | Acción | Qué decir | Tiempo |
|---|---|---|---:|
| 1 | `cat cie2026-zephyr/apps/blinky_boton/src/main.c` (pasar rápido) | "Es el código de la diapositiva." | 0:30 |
| 2 | `west build -p -b nrf5340dk/nrf5340/cpuapp cie2026-zephyr/apps/blinky_boton` **en vivo** | Mientras compila: "Está procesando el devicetree, eligiendo drivers y compilando el kernel." | 1:00 |
| 3 | `grep -A8 "aliases {" build/zephyr/zephyr.dts` | "`led0` y `sw0` ya vienen en el `.dts` de la placa: no escribí ni un overlay." | 0:30 |
| 4 | `west flash` y pulsar el botón | "El LED cambia de velocidad." | 0:45 |
| 5 | `west flash -d build/blinky-k64f -r pyocd` | "Mismo `main.c`, otro fabricante." | 0:45 |
| 6 | `west flash -d build/blinky-c3` y `west flash -d build/blinky-doit` | "Ahora en RISC-V y en Xtensa. Ninguno es ARM." | 1:15 |
| 7 | `west flash -d build/blinky-teensy` | "La Teensy no tiene botón: la app lo supo al compilar y parpadea fijo." | 0:45 |
| 8 | `west flash -d build/acel-k64f -r pyocd` y abrir la consola | "Acelerómetro integrado: aceleración y campo magnético." Inclinar la placa. | 1:15 |
| 9 | `west flash -d build/acel-mcxn -r linkserver` | "Otro chip, otro driver, mismo `main.c`: acá aparece aceleración y temperatura." | 1:00 |
| 10 | `cat cie2026-zephyr/apps/acelerometro/mpu6050.overlay` y `west flash -d build/acel-nrf` | "El nRF5340 DK no trae acelerómetro: le agrego un MPU6050 con este overlay. Aparecen giro y temperatura." Mover la placa. | 1:45 |
| 11 | En la consola del nRF: `i2c scan` y `sensor get mpu6050@68` | "La shell viene en el `prj.conf`: exploro el bus sin recompilar." | 1:15 |
| 12 | `cat` de `temperatura/boards/esp32c3_supermini.overlay` y `frdm_mcxn236.overlay` | "Dos formas muy distintas de medir temperatura; la app no se entera." | 1:00 |
| 13 | `west flash -d build/temp-c3`, `-d build/temp-teensy` y `-d build/temp-mcxn -r linkserver` | Apoyar un dedo sobre el chip y ver cómo sube la lectura. | 2:00 |

Total: unos 14 min. Quedan unos 3 min de margen.

### Plan B

- **Falla la compilación en vivo:** usar un build ya hecho y decir "hice uno antes".
- **No graba una placa:** pasar a la siguiente y decir "esto también pasa en la vida real".
- **El MPU6050 no responde:** usar `i2c scan` en la shell para mostrar el
  diagnóstico (es parte de lo que se quiere enseñar) y seguir con la K64F.
- **Falla todo el banco (USB, alimentación):** reproducir el video y mostrar en la
  terminal cómo compila la misma app para varias placas (no depende del hardware).

---

## Lista de materiales

- Notebook con el workspace instalado y probado, cargador
- Adaptador HDMI para el proyector
- Hub USB con alimentación propia
- Placas: nRF5340 DK, FRDM-K64F, FRDM-MCXN236, ESP32-C3 SuperMini,
  DOIT ESP32 DevKit V1, Teensy 4.1 (LPCXpresso55S69 de reserva)
- 1 MPU6050 con cables dupont (nRF5340 DK), y uno de repuesto
- Adaptador USB-serie de 3,3 V (consola de la Teensy)
- Cables USB rotulados (micro-USB, USB-C)
- Video de respaldo en la notebook y en un pendrive
