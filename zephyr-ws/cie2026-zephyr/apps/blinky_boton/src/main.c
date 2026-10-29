/*
 * blinky_boton - Laboratorio de Sistemas Embebidos, UTN FRA
 *
 * Un LED parpadea; cada pulsación del botón cambia la velocidad.
 *
 * Este archivo NO sabe en qué placa corre: solo conoce los alias
 * "led0" y "sw0" del devicetree. Cada placa decide a qué pin físico
 * apunta cada alias. Si la placa no tiene botón (sw0), el LED parpadea
 * igual, a velocidad fija.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(blinky_boton, LOG_LEVEL_INF);

/* Puerto, pin y flags (ej. activo en bajo) salen del devicetree. */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

static const int periodos_ms[] = {500, 250, 100, 50};
static atomic_t indice;

#if DT_NODE_EXISTS(DT_ALIAS(sw0))
static const struct gpio_dt_spec boton = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
static struct gpio_callback boton_cb;

static void boton_presionado(const struct device *dev, struct gpio_callback *cb,
			     uint32_t pins)
{
	atomic_val_t i = (atomic_get(&indice) + 1) % ARRAY_SIZE(periodos_ms);

	atomic_set(&indice, i);
	LOG_INF("Boton: periodo = %d ms", periodos_ms[i]);
}

static int configurar_boton(void)
{
	if (!gpio_is_ready_dt(&boton)) {
		return -ENODEV;
	}
	gpio_pin_configure_dt(&boton, GPIO_INPUT);
	gpio_pin_interrupt_configure_dt(&boton, GPIO_INT_EDGE_TO_ACTIVE);
	gpio_init_callback(&boton_cb, boton_presionado, BIT(boton.pin));
	gpio_add_callback_dt(&boton, &boton_cb);
	LOG_INF("Boton: %s pin %d", boton.port->name, boton.pin);
	return 0;
}
#else
static int configurar_boton(void)
{
	LOG_INF("Esta placa no tiene boton (sw0): periodo fijo");
	return 0;
}
#endif

int main(void)
{
	LOG_INF("blinky_boton en %s (SoC %s)", CONFIG_BOARD_TARGET, CONFIG_SOC);

	if (!gpio_is_ready_dt(&led) || configurar_boton() != 0) {
		LOG_ERR("GPIO no disponible");
		return -ENODEV;
	}

	gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
	LOG_INF("LED  : %s pin %d", led.port->name, led.pin);

	while (1) {
		gpio_pin_toggle_dt(&led);
		k_msleep(periodos_ms[atomic_get(&indice)]);
	}

	return 0;
}
