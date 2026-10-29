/*
 * temperatura - Laboratorio de Sistemas Embebidos, UTN FRA
 *
 * Lee el sensor de temperatura interno del microcontrolador cada dos
 * segundos.
 *
 * La aplicación solo conoce el alias "die-temp0", el nombre estándar que
 * usa Zephyr para el sensor de temperatura del chip. Cada SoC lo implementa
 * distinto (un canal del ADC, un periférico dedicado...), pero la
 * aplicación siempre pide lo mismo: SENSOR_CHAN_DIE_TEMP.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(temperatura, LOG_LEVEL_INF);

#if !DT_NODE_EXISTS(DT_ALIAS(die_temp0))
#error "Falta el alias 'die-temp0': esta placa no tiene sensor de temperatura interno soportado"
#endif

static const struct device *const sensor = DEVICE_DT_GET(DT_ALIAS(die_temp0));

int main(void)
{
	struct sensor_value temp;

	LOG_INF("temperatura en %s (SoC %s)", CONFIG_BOARD_TARGET, CONFIG_SOC);

	if (!device_is_ready(sensor)) {
		LOG_ERR("El sensor %s no esta listo", sensor->name);
		return -ENODEV;
	}
	LOG_INF("Sensor: %s", sensor->name);

	while (1) {
		if (sensor_sample_fetch_chan(sensor, SENSOR_CHAN_DIE_TEMP) < 0 ||
		    sensor_channel_get(sensor, SENSOR_CHAN_DIE_TEMP, &temp) < 0) {
			LOG_WRN("Error al leer el sensor");
		} else {
			LOG_INF("Temperatura del chip: %.1f C", sensor_value_to_double(&temp));
		}
		k_msleep(2000);
	}

	return 0;
}
