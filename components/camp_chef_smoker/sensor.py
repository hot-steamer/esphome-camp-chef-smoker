import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, sensor, text_sensor, binary_sensor
from esphome.const import *  # from esphome.const import ICON_EMPTY, UNIT_EMPTY

DEPENDENCIES = ["uart"]

camp_ns = cg.esphome_ns.namespace("camp_chef_smoker")

CampChefSmoker = camp_ns.class_(
    "CampChefSmoker",
    cg.PollingComponent,
    uart.UARTDevice
)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(CampChefSmoker),

    cv.Optional(CONF_UPDATE_INTERVAL, default="5s"): cv.update_interval,

    cv.Optional("pit_temperature"): sensor.sensor_schema(
        unit_of_measurement="°F",
        accuracy_decimals=0,
    ),

    cv.Optional("set_temperature"): sensor.sensor_schema(
        unit_of_measurement="°F",
        accuracy_decimals=0,
    ),

    cv.Optional("probe1_temperature"): sensor.sensor_schema(
        unit_of_measurement="°F",
        accuracy_decimals=0,
    ),

    cv.Optional("probe2_temperature"): sensor.sensor_schema(
        unit_of_measurement="°F",
        accuracy_decimals=0,
    ),

    cv.Optional("smoke_level"): sensor.sensor_schema(
        accuracy_decimals=0,
    ),

    cv.Optional("startup_timer"): sensor.sensor_schema(
        unit_of_measurement="s"
    ),

    cv.Optional("feed_timer"): sensor.sensor_schema(
        unit_of_measurement="s"
    ),

    cv.Optional("shutdown_timer"): sensor.sensor_schema(
        unit_of_measurement="s"
    ),

    cv.Optional("active_countdown_timer_seconds"): sensor.sensor_schema(
        unit_of_measurement="s"
    ),

    cv.Optional("active_countdown_timer"): text_sensor.text_sensor_schema(),

    cv.Optional("cook_time_seconds"): sensor.sensor_schema(
        unit_of_measurement="s"
    ),

    cv.Optional("cook_time"): text_sensor.text_sensor_schema(),

    cv.Optional("status"): text_sensor.text_sensor_schema(),

    cv.Optional("flame_out"): binary_sensor.binary_sensor_schema(),
    cv.Optional("over_temp"): binary_sensor.binary_sensor_schema(),
    cv.Optional("rtd_error"): binary_sensor.binary_sensor_schema(),

    cv.Optional("probe1_valid"): binary_sensor.binary_sensor_schema(),
    cv.Optional("probe2_valid"): binary_sensor.binary_sensor_schema(),

    cv.Optional("auger_state"): binary_sensor.binary_sensor_schema(),
    cv.Optional("fan_state"): binary_sensor.binary_sensor_schema(),
    cv.Optional("igniter_state"): binary_sensor.binary_sensor_schema(),

}).extend(cv.polling_component_schema("5s")).extend(
    uart.UART_DEVICE_SCHEMA
)


async def to_code(config):

    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    if "pit_temperature" in config:
        sens = await sensor.new_sensor(config["pit_temperature"])
        cg.add(var.set_pit_temp_sensor(sens))

    if "set_temperature" in config:
        sens = await sensor.new_sensor(config["set_temperature"])
        cg.add(var.set_set_temp_sensor(sens))

    if "probe1_temperature" in config:
        sens = await sensor.new_sensor(config["probe1_temperature"])
        cg.add(var.set_probe1_sensor(sens))

    if "probe2_temperature" in config:
        sens = await sensor.new_sensor(config["probe2_temperature"])
        cg.add(var.set_probe2_sensor(sens))

    if "smoke_level" in config:
        sens = await sensor.new_sensor(config["smoke_level"])
        cg.add(var.set_smoke_sensor(sens))

    if "startup_timer" in config:
        sens = await sensor.new_sensor(config["startup_timer"])
        cg.add(var.set_startup_timer_sensor(sens))

    if "feed_timer" in config:
        sens = await sensor.new_sensor(config["feed_timer"])
        cg.add(var.set_feed_timer_sensor(sens))

    if "shutdown_timer" in config:
        sens = await sensor.new_sensor(config["shutdown_timer"])
        cg.add(var.set_shutdown_timer_sensor(sens))

    if "active_countdown_timer_seconds" in config:
        sens = await sensor.new_sensor(config["active_countdown_timer_seconds"])
        cg.add(var.set_active_countdown_timer_seconds_sensor(sens))

    if "active_countdown_timer" in config:
        txt = await text_sensor.new_text_sensor(config["active_countdown_timer"])
        cg.add(var.set_active_countdown_timer_sensor(txt))

    if "cook_time_seconds" in config:
        sens = await sensor.new_sensor(config["cook_time_seconds"])
        cg.add(var.set_cook_time_seconds_sensor(sens))

    if "cook_time" in config:
        txt = await text_sensor.new_text_sensor(config["cook_time"])
        cg.add(var.set_cook_time_sensor(txt))

    if "status" in config:
        txt = await text_sensor.new_text_sensor(config["status"])
        cg.add(var.set_status_sensor(txt))

    if "flame_out" in config:
        b = await binary_sensor.new_binary_sensor(config["flame_out"])
        cg.add(var.set_flame_out_sensor(b))

    if "over_temp" in config:
        b = await binary_sensor.new_binary_sensor(config["over_temp"])
        cg.add(var.set_over_temp_sensor(b))

    if "rtd_error" in config:
        b = await binary_sensor.new_binary_sensor(config["rtd_error"])
        cg.add(var.set_rtd_error_sensor(b))

    if "probe1_valid" in config:
        b = await binary_sensor.new_binary_sensor(config["probe1_valid"])
        cg.add(var.set_probe1_valid_sensor(b))

    if "probe2_valid" in config:
        b = await binary_sensor.new_binary_sensor(config["probe2_valid"])
        cg.add(var.set_probe2_valid_sensor(b))
    
    if "auger_state" in config:
        b = await binary_sensor.new_binary_sensor(config["auger_state"])
        cg.add(var.set_auger_sensor(b))
    
    if "fan_state" in config:
        b = await binary_sensor.new_binary_sensor(config["fan_state"])
        cg.add(var.set_fan_sensor(b))

    if "igniter_state" in config:
        b = await binary_sensor.new_binary_sensor(config["igniter_state"])
        cg.add(var.set_igniter_sensor(b))