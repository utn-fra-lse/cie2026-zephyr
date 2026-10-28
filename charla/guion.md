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
| 31–47 | **Demo en vivo** | "El banco de pruebas" → "Resultados" | 16 min, con plan B |
| 47–53 | Herramientas, límites, cierre | "west" → "Probalo vos" | — |
| 53–60 | Preguntas | "¿Preguntas?" (volver al QR) | colchón real |

Si vas atrasado al minuto 31: saltear "APIs estandarizadas".
Si vas atrasado al 47: ir directo a "Para llevarse".

---

## Bloque 1 · Apertura y problema (0–5)

**Portada.** Presentarse en una línea y presentar el LSE. Pregunta a la sala:
*"¿Quién programó alguna vez un micro con el SDK del fabricante? ¿Y quién tuvo
que pasar ese proyecto a otro micro?"* (manos arriba).

**Hoja de ruta.** 20 segundos. Anticipar que hay demo en vivo con cinco
fabricantes.

**Prender un LED, cuatro fabricantes.** Dejar que lean. Remarcar que no es solo
la sintaxis: el pin y el puerto están escritos en el código, y además cambian el
build, el IDE y la configuración de relojes.

**El costo de casarse con un fabricante.** Contar una anécdota propia (faltante
de chips, cambio de placa en un proyecto de cátedra o de la industria). El
diagrama muestra que la lógica de la app y la HAL quedan mezcladas.

**Standout.** Pausa de dos segundos. *"Eso es lo que vamos a ver hoy."*

## Bloque 2 · Zephyr en cinco minutos (5–12)

**¿Qué es Zephyr?** Las cifras salen del árbol de v4.4.2: más de 1000 placas,
160 fabricantes y 11 arquitecturas. Remarcar que lo impulsan los propios
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

**Bindings.** El YAML valida. Mencionar que hay más de 3700.

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

## Bloque 5 · Demo (31–47)

**El banco de pruebas.** Mostrar físicamente las placas sobre la mesa. Remarcar los
ESP32: el C3 es **RISC-V** y el S3 es **Xtensa**. Ninguno de los dos es ARM.

**App 1, `main.c`.** *"Ni un número de pin."*

**El overlay.** La Pico no tiene botón, así que se le agrega uno en unas pocas
líneas. Las demás placas no necesitan overlay.

**App 2, `main.c`.** La app ni siquiera sabe qué sensor tiene: pregunta por
aceleración, giro, campo magnético y temperatura, y muestra lo que el driver
responda.

**Placa + sensor.** N + M, no N × M. El overlay del MPU6050 se aplica después del
de la placa, por eso en la K64F reemplaza al integrado.

**Mismo `main.c`, distinto sensor.** La tabla anticipa lo que se va a ver en vivo.
Remarcar que FXOS8700 y MMA8652FC son dos chips con **un solo driver**, y que el
modo lo elige la configuración de cada placa dentro de Zephyr.

**Standout "Demo en vivo"** → cambiar a la terminal (ver procedimiento abajo).

**Resultados.** Volver a las diapositivas. Misma app, flash y RAM distintos según
la arquitectura; pocas líneas de overlay.

**La shell.** Si la demo vino bien de tiempo, ya se usó en vivo. Si no, mostrar la
diapositiva y seguir.

## Bloque 6 · Herramientas, límites y cierre (47–53)

**west.** Un solo comando de flash para programadores de cinco fabricantes.

**Lo que Zephyr no resuelve.** Importante para la credibilidad. *"Portar no es
validar."*

**Para llevarse.** Cuatro puntos. Leerlos.

**Probalo vos.** Dejar el QR en pantalla durante las preguntas.

**¿Preguntas?** Volver a "Probalo vos" (QR) mientras se responde.

---

## Procedimiento de la demo (16 min)

### Preparación (la noche anterior)

- [ ] Workspace actualizado en la notebook (`west update`) y compilaciones
      verificadas.
- [ ] **Builds hechos de antemano** en directorios separados (`-d`), para que en
      vivo solo se compile **uno** (la Pico, unos 15 s).
- [ ] Cada placa grabada con `blinky_boton` y funcionando (punto de partida conocido).
- [ ] MPU6050 cableados y probados en Black Pill, ESP32-C3 y nRF5340 DK, y uno
      suelto con cables para conectar en vivo a la FRDM-K64F.
- [ ] Hub USB con alimentación externa. Rotular cada cable con el nombre de la placa.
- [ ] Terminal con dos paneles (o dos ventanas): comandos arriba y consola serie abajo.
- [ ] Fuente grande en la terminal (≥ 20 pt), tema claro para el proyector.
- [ ] Video de respaldo grabado (plan B).

Builds de antemano (desde `~/zephyr-ws`):

```bash
west build -p -b blackpill_f401cc -d build/blinky-bp cie2026-zephyr/apps/blinky_boton
west build -p -b nrf5340dk/nrf5340/cpuapp -d build/blinky-nrf cie2026-zephyr/apps/blinky_boton
west build -p -b frdm_k64f -d build/blinky-k64f cie2026-zephyr/apps/blinky_boton
west build -p -b esp32c3_supermini -d build/blinky-c3 cie2026-zephyr/apps/blinky_boton
west build -p -b esp32s3_devkitc/esp32s3/procpu -d build/blinky-s3 cie2026-zephyr/apps/blinky_boton

west build -p -b frdm_k64f -d build/acel-k64f cie2026-zephyr/apps/acelerometro
west build -p -b frdm_k64f -d build/acel-k64f-mpu cie2026-zephyr/apps/acelerometro -- -DEXTRA_DTC_OVERLAY_FILE=mpu6050.overlay
west build -p -b nrf5340dk/nrf5340/cpuapp -d build/acel-nrf cie2026-zephyr/apps/acelerometro -- -DEXTRA_DTC_OVERLAY_FILE=mpu6050.overlay
west build -p -b esp32c3_supermini -d build/acel-c3 cie2026-zephyr/apps/acelerometro -- -DEXTRA_DTC_OVERLAY_FILE=mpu6050.overlay
west build -p -b blackpill_f401cc -d build/acel-bp cie2026-zephyr/apps/acelerometro -- -DEXTRA_DTC_OVERLAY_FILE=mpu6050.overlay
```

### En vivo

| # | Acción | Qué decir | Tiempo |
|---|---|---|---:|
| 1 | `cat cie2026-zephyr/apps/blinky_boton/src/main.c` (pasar rápido) | "Es el código de la diapositiva." | 0:30 |
| 2 | `west build -p -b rpi_pico cie2026-zephyr/apps/blinky_boton` **en vivo** | Mientras compila: "Está procesando el devicetree, eligiendo drivers y compilando el kernel." | 1:00 |
| 3 | `grep -A6 "aliases {" build/zephyr/zephyr.dts` | "Este es el árbol final: `sw0` viene de nuestro overlay." | 0:45 |
| 4 | BOOTSEL + copiar `build/zephyr/zephyr.uf2` a la Pico; pulsar el botón | "El LED cambia de velocidad." | 1:00 |
| 5 | `west flash -d build/blinky-nrf` y `west flash -d build/blinky-k64f -r pyocd` | "Mismo `main.c`, otro fabricante, sin overlay." | 1:30 |
| 6 | `west flash -d build/blinky-c3` y `west flash -d build/blinky-s3` | "Ahora en RISC-V y en Xtensa. Ninguno es ARM." | 1:15 |
| 7 | `west flash -d build/acel-k64f -r pyocd` y abrir la consola | "Acelerómetro integrado: aceleración y campo magnético." Inclinar la placa. | 1:30 |
| 8 | `cat cie2026-zephyr/apps/acelerometro/mpu6050.overlay` | "Esto es todo lo que hace falta para agregar el MPU6050." | 0:45 |
| 9 | Conectar el MPU6050 a la K64F y `west flash -d build/acel-k64f-mpu -r pyocd` | "Misma placa, mismo `main.c`: ahora hay giro y temperatura, y no hay campo magnético." | 2:00 |
| 10 | `west flash -d build/acel-nrf` (o `acel-c3`) | "Y el mismo MPU6050 en otro fabricante." Mover la placa. | 1:30 |
| 11 | En la consola: `i2c scan` y `sensor get mpu6050@68` | "La shell viene en el `prj.conf`: exploro el bus sin recompilar." | 2:00 |

Total: unos 14 min. Quedan unos 2 min de margen.

### Plan B

- **Falla la compilación en vivo:** usar un build ya hecho y decir "hice uno antes".
- **No graba una placa:** pasar a la siguiente y decir "esto también pasa en la vida real".
- **El MPU6050 no responde:** usar `i2c scan` en la shell para mostrar el
  diagnóstico (es parte de lo que se quiere enseñar) y seguir con otra placa.
- **Falla todo el banco (USB, alimentación):** reproducir el video y mostrar en la
  terminal cómo compila la misma app para varias placas (no depende del hardware).

---

## Lista de materiales

- Notebook con el workspace instalado y probado, cargador
- Adaptador HDMI para el proyector
- Hub USB con alimentación propia
- Placas: Pico (+ pulsador en GP15), Black Pill F401 (+ adaptador USB-serie),
  ESP32-C3 SuperMini, ESP32-S3 DevKitC (+ LED en GPIO10), nRF5340 DK, FRDM-K64F
- Sensores: 4 MPU6050, protoboards y cables dupont
- Cables USB rotulados (micro-USB, USB-C)
- Video de respaldo en la notebook y en un pendrive
