/**
 * \file main.c
 * \author Adam Zvara <xzvara01@stud.fit.vutbr.cz>
 * \brief ESP32 project for setting LED intensity with brightness sensor using I2C communication
 * \date 2022-12
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "driver/i2c.h"
#include "driver/uart.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_err.h"

/** LEDC timer settings **/
#define LEDC_HS_TIMER          LEDC_TIMER_0
#define LEDC_HS_MODE           LEDC_HIGH_SPEED_MODE
#define LEDC_HS_CH0_GPIO       26   /*!< GPIO PIN number of LED 1 */
#define LEDC_HS_CH0_CHANNEL    LEDC_CHANNEL_0
#define LEDC_HS_CH1_GPIO2      25   /*!< GPIO PIN number of LED 2 */
#define LEDC_HS_CH1_CHANNEL    LEDC_CHANNEL_1
/** LED parameters **/
#define LEDC_DUTY         8192      /*!< [0, 2^duty_resolution-1] => (0, 8192)          */
#define LEDC_FADE_TIME    1000      /*!< Time after which one fade cycle is finished    */
#define LEDC_DIV_FACTOR   6000     /*!< Divisor when calculating new duty */
// LEDC_DIV_FACTOR represents how much light is needed to see change in LED brightness, the higher the quicker LED changes brightness
/** I2C settings **/
#define I2C_MASTER_SCL_IO           22      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           21      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0       /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define ACK_CHECK_EN                1
#define ACK_VAL                     0
#define NACK_VAL                    1
/** Sensor specififc settings **/
#define BH1750_SENSOR_ADDR         0x23    /*!< Slave address (LOW) of the BH1750 sensor */
#define BH1750_SENSOR_FREQ         400000  /*!< SCL frequency of the BH1750 sensor       */
#define BH1750_SENZOR_RES          0x10    /*!< Measurement mode of BH1750 sensor        */
/** UART Settings **/
#define UART_BAUD_RATE             115200
#define UART_BUF_SIZE              1024
#define UART_PORT_NUM              UART_NUM_0
/** NVS SETTINGS **/
#define NVS_TRESHOLD_NAME          "treshold"
/** Others settings **/
#define DEBUG 0

/** Global variables **/
float lux;          /*!< Global variable containing light intensity from sensor */

/** Links to examples for different parts of application
 *
 * UART:
 * https://github.com/espressif/esp-idf/blob/master/examples/peripherals/uart/uart_echo/main/uart_echo_example_main.c
 *
 * I2C:
 * https://github.com/espressif/esp-idf/blob/master/examples/peripherals/i2c/i2c_self_test/main/i2c_example_main.c
 *
 * LEDC:
 * https://github.com/espressif/esp-idf/blob/master/examples/peripherals/ledc/ledc_fade/main/ledc_fade_example_main.c
 *
 * NVS:
 * https://github.com/espressif/esp-idf/blob/master/examples/storage/nvs_rw_value/main/nvs_value_example_main.c
 *
 */

/** LED 1 Configuration */
static const ledc_channel_config_t ledc_channel1 = {
    .channel    = LEDC_HS_CH0_CHANNEL,
    .duty       = 0,
    .gpio_num   = LEDC_HS_CH0_GPIO,
    .speed_mode = LEDC_HS_MODE,
    .hpoint     = 0,
    .timer_sel  = LEDC_HS_TIMER,
    .flags.output_invert = 0
};

/** LED 2 Configuration */
static const ledc_channel_config_t ledc_channel2 = {
    .channel    = LEDC_HS_CH1_CHANNEL,
    .duty       = 0,
    .gpio_num   = LEDC_HS_CH1_GPIO2,
    .speed_mode = LEDC_HS_MODE,
    .hpoint     = 0,
    .timer_sel  = LEDC_HS_TIMER,
    .flags.output_invert = 0
};

/**
 * \brief Initializes I2C master
 * \details This function initializes I2C master for communication with BH1750
 * light intensity sensor
 */
void i2c_master_init()
{
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = BH1750_SENSOR_FREQ,
    };
    ESP_ERROR_CHECK(i2c_param_config(i2c_master_port, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0));
}

/**
 * \brief Intialize LEDC timer
 */
void ledc_timer_init()
{
    /* LEDC timer configuration, which is the 1st step of setting up LEDC module */
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_13_BIT, // resolution of PWM duty
        .freq_hz = 5000,                      // frequency of PWM signal
        .speed_mode = LEDC_HS_MODE,           // timer mode
        .timer_num = LEDC_HS_TIMER,           // timer index
        .clk_cfg = LEDC_AUTO_CLK,             // auto select the source clock
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer)); // Apply timer configuration
}

/**
 * \brief Intialize LEDC
 */
void ledc_channel_init(const ledc_channel_config_t *ledc)
{
    ESP_ERROR_CHECK(ledc_channel_config(ledc)); // Apply channel configuration
}

/**
 * \brief Initialize UART
 */
void uart_init()
{
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, UART_BUF_SIZE, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, GPIO_NUM_1, GPIO_NUM_3, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
}

/**
 * \brief Initialize NVS
 */
void nvs_init()
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }
}

/**
 * \brief Read value from NVS
 *
 * \param[in] Name of stored variable
 * \return Value of stored variable
 */
int nvs_read_value(char *name)
{
    nvs_handle_t handle;
    int value;
    int err = nvs_open("storage", NVS_READONLY, &handle);
    if (err == ESP_OK) {
        ESP_ERROR_CHECK(nvs_get_i32(handle, name, &value));
    }
    nvs_close(handle);
    return value;
}

/**
 * \brief Store value in NVS
 *
 * \param[in] name  Name of variable to store
 * \param[in] value Value of variable
 */
void nvs_write_value(char *name, int value)
{
    nvs_handle_t handle;
    int err = nvs_open("storage", NVS_READWRITE, &handle);
    if (err == ESP_OK) {
        ESP_ERROR_CHECK(nvs_set_i32(handle, name, value));
        ESP_ERROR_CHECK(nvs_commit(handle));
    }
    nvs_close(handle);
}

/**
 * \brief Set LEDC duty
 * \details This function sets the LEDC duty to given value, which changes the brightness of the LED
 *
 * \param[in] c     LEDC channgel configuration
 * \param[in] duty  New LEDC duty value
 */
void set_led(const ledc_channel_config_t *c, int duty)
{
    ESP_ERROR_CHECK(ledc_set_fade_time_and_start(c->speed_mode, c->channel, duty, LEDC_FADE_TIME / portTICK_PERIOD_MS, LEDC_FADE_NO_WAIT));
}

/**
 * \brief Read data from slave (light sensor) into master using I2C
 *
 * \param[out] data Array to store results into (2x uint8 values, MSB bit on first position)
 * \return ESP_OK on success, otherwise an error code
 */
void master_read_slave(uint8_t data[2])
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BH1750_SENSOR_ADDR << 1) | I2C_MASTER_READ, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, &(data[0]), ACK_VAL);
    i2c_master_read_byte(cmd, &(data[1]), NACK_VAL);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    // Wait 120ms untill new measurement is ready
    vTaskDelay(120 / portTICK_PERIOD_MS);
}

/**
 * \brief Set sensor measurement resolution
 * \details This function is following the documentation of sensor, page 7/17
 *   step 1, setting up continuous H-resolution mode, for other modes, change BH1750_SENZOR_RES
 */
void sensor_set_res()
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BH1750_SENSOR_ADDR << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, BH1750_SENZOR_RES, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    // Wait for 180ms (maximum value) for measurement to complete
    vTaskDelay(180 / portTICK_PERIOD_MS);
}

/**
 * \brief Send UART prompt message
 */
void uart_send_prompt()
{
    char *prompt = "\nZadajte prah zapnutia druhej diódy: ";
    uart_write_bytes(UART_PORT_NUM, prompt, strlen(prompt));
}

/**
 * \brief Main sensor task function
 * \details This function reads values from sensor and updates brightness of LED
 */
void sensor_task()
{
    uint8_t data[2];
#if DEBUG
    static float old_lux = 0;
#endif
    while(1) {
        master_read_slave(data);
        if (!ESP_OK) {
            /* Calculate new lux value, devide by 1.2 acording to documentation */
            lux = ((data[0] << 8 | data[1]) / 1.2);
#if DEBUG
            if (old_lux - lux > 1) {
                printf("sensor val: %d [Lux]\n", (int)(lux));
            }
            old_lux = lux;
#endif
       }
    }
}

/**
 * \brief Main uart task function
 * \details This function is responsible for communication with user using UART protocol.
 * It reads data from UART buffer and waits for newline to parse user input.
 */
void uart_task()
{
    // Buffer for receiving data from UART
    uint8_t *rec = (uint8_t *) malloc(UART_BUF_SIZE);
    // Buffer for acummulating data from UART
    uint8_t *data = (uint8_t *) malloc(UART_BUF_SIZE);
    int data_len = 0; // current data length
    // Send user prompt message
    uart_send_prompt();

    while (1) {
        // Read data from the UART
        int len = uart_read_bytes(UART_PORT_NUM, rec, (UART_BUF_SIZE - 1), 20 / portTICK_PERIOD_MS);

        if (rec[len-1] == '\n') {
            // New user message has been received
            data[data_len] = '\0';
            int new_treshold = atoi((const char *)data);
            // Store treshold value in NVS
            nvs_write_value(NVS_TRESHOLD_NAME, new_treshold);
            // Clear stored message and send prompt again
            memset(data, 0, data_len);
            data_len = 0;
            uart_send_prompt();
        } else {
            // Normal chars received, store them in data buffer
            uart_write_bytes(UART_PORT_NUM, rec, len); // user feedback after char sent
            memcpy(data+data_len, rec, len);
            data_len += len;
        }
    }
}

/**
 * \brief Main LED task
 * \details This task is responsible for controlling brightness of both LEDS.
 * It reads values from NVS to set treshold for LED2 and updates duty for LED1
 * based on global variable lux
 */
void led_task()
{
    int new_duty;
    while (1) {
        int led2_treshold = nvs_read_value(NVS_TRESHOLD_NAME);
        // LED1
        /* We need to map lux values from sensor (from documention values in range 0-65535) to
            LEDC_DUTY, which is in range 0-8192 so new_duty (should) = lux * LEDC_DUTY / 65535
            But it is very hard to actually reach maximum brigtness on sensor, usual average is around few hundereds lux,
            and after applying direct light around 20-30k lux, but to actually see difrences in LED brightness, we devide by only
            LEDC_DIV_FACTOR and truncate higher brightness levels*/
        new_duty = (lux / LEDC_DIV_FACTOR) > 1 ? LEDC_DUTY : LEDC_DUTY * lux / LEDC_DIV_FACTOR;
        set_led(&ledc_channel1, new_duty);
        // LED2
        if (lux > led2_treshold) {
            set_led(&ledc_channel2, LEDC_DUTY);
        } else {
            set_led(&ledc_channel2, 0);
        }
    }
}

/** Main function **/
void app_main(void)
{
    /* Initialize LEDCs */
    ledc_timer_init();
    ledc_channel_init(&ledc_channel1);
    ledc_channel_init(&ledc_channel2);
    ESP_ERROR_CHECK(ledc_fade_func_install(0));

    /* I2C initialization */
    i2c_master_init();

    /* UART initialization */
    uart_init();

    /* NVS initialization */
    nvs_init();
#if DEBUG
    int initial_treshold = nvs_read_value(NVS_TRESHOLD_NAME);
    printf("\nPočiatočná úroveň prahu osvetlenia: %d\n", initial_treshold);
#endif
    /* This function needs to be called in order for the sensor to be working
    as per documentation of the sensor, page 4 */
    sensor_set_res();

    /* Create task to read sensor values and apply them to led */
    xTaskCreate(&sensor_task, "sensor task", 2048, NULL, 10, NULL);
    xTaskCreate(&uart_task, "uart task", 2048, NULL, 10, NULL);
    xTaskCreate(&led_task, "led task", 2048, NULL, 10, NULL);
}