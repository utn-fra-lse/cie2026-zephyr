/*
 * acelerometro - Laboratorio de Sistemas Embebidos, UTN FRA
 *
 * Lee periódicamente "el sensor" y muestra todo lo que el driver sepa
 * medir: aceleración, velocidad angular, campo magnético, temperatura
 * del chip.
 *
 * La aplicación no nombra ningún sensor ni ninguna placa. Solo conoce el
 * alias "accel0", el nombre estándar que usa Zephyr para el acelerómetro
 * de una placa, y la API genérica de sensores. El mismo código muestra
 * aceleración + giro con un MPU6050 externo, aceleración + campo magnético
 * con el FXOS8700 de la FRDM-K64F, aceleración + temperatura con el
 * FXLS8974 de la FRDM-MCXN236, o solo aceleración con el MMA8652FC de la
 * LPCXpresso55S69.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(acelerometro, LOG_LEVEL_INF);

#if !DT_NODE_EXISTS(DT_ALIAS(accel0))
#error "Falta el alias 'accel0': esta placa no tiene acelerometro integrado. \
Compilá con -DEXTRA_DTC_OVERLAY_FILE=mpu6050.overlay"
#endif

static const struct device *const sensor = DEVICE_DT_GET(DT_ALIAS(accel0));

/* LED de "latido": opcional, si la placa no define led0 se omite. */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led0), gpios, {0});

struct magnitud {
	enum sensor_channel canal;
	int ejes; /* 3 para X/Y/Z, 1 para un escalar */
	const char *nombre;
	const char *unidad;
};

/* Todo lo que la app sabe mostrar; cada driver responde lo que tiene
 * y devuelve -ENOTSUP para el resto. */
static const struct magnitud magnitudes[] = {
	{SENSOR_CHAN_ACCEL_XYZ, 3, "Aceleracion", "m/s2"},
	{SENSOR_CHAN_GYRO_XYZ, 3, "Giro", "rad/s"},
	{SENSOR_CHAN_MAGN_XYZ, 3, "Campo magn.", "G"},
	{SENSOR_CHAN_DIE_TEMP, 1, "Temp. chip", "C"},
	{SENSOR_CHAN_AMBIENT_TEMP, 1, "Temperatura", "C"},
};

static void mostrar(const struct magnitud *m)
{
	struct sensor_value v[3];

	if (sensor_channel_get(sensor, m->canal, v) != 0) {
		return; /* este sensor no mide esta magnitud */
	}

	if (m->ejes == 3) {
		LOG_INF("%-12s x=%8.3f y=%8.3f z=%8.3f %s", m->nombre,
			sensor_value_to_double(&v[0]), sensor_value_to_double(&v[1]),
			sensor_value_to_double(&v[2]), m->unidad);
	} else {
		LOG_INF("%-12s %8.3f %s", m->nombre, sensor_value_to_double(&v[0]), m->unidad);
	}
}

int main(void)
{
	LOG_INF("acelerometro en %s (SoC %s)", CONFIG_BOARD_TARGET, CONFIG_SOC);

	if (!device_is_ready(sensor)) {
		LOG_ERR("El sensor %s no respondio (cableado? direccion I2C?)", sensor->name);
		return -ENODEV;
	}
	LOG_INF("Sensor: %s", sensor->name);

	if (led.port != NULL && gpio_is_ready_dt(&led)) {
		gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
	}

	while (1) {
		if (sensor_sample_fetch(sensor) < 0) {
			LOG_WRN("Error al leer el sensor");
		} else {
			for (size_t i = 0; i < ARRAY_SIZE(magnitudes); i++) {
				mostrar(&magnitudes[i]);
			}
		}

		if (led.port != NULL) {
			gpio_pin_toggle_dt(&led);
		}
		k_msleep(2000);
	}

	return 0;
}
