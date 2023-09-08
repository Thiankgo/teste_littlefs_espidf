#include <stdio.h>

#include <string>

#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_littlefs.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void println(const char *message) {
  printf("%s\n", message);
}

void print(const char *message) {
  printf("%s", message);
}

extern "C" {
void app_main(void);
}

#include "esp_littlefs.h"
#include "esp_log.h"
#include "esp_system.h"

static const char *TAG = "file_example";
static const char *FILENAME = "/littlefs/hello.txt";  // Caminho para o arquivo no sistema de arquivos LittleFS
static const char *DATA = "Hello world";              // Dados a serem escritos no arquivo

esp_err_t init_littlefs() {
  esp_err_t ret;

  ESP_LOGI(TAG, "Initializing LittleFS");

  esp_vfs_littlefs_conf_t conf = {
      .base_path = "/littlefs",
      .partition_label = "littlefs",
      .format_if_mount_failed = true,
      .dont_mount = false,
  };

  ret = esp_vfs_littlefs_register(&conf);

  if (ret != ESP_OK) {
    if (ret == ESP_FAIL) {
      ESP_LOGE(TAG, "Failed to mount or format filesystem");
    } else if (ret == ESP_ERR_NOT_FOUND) {
      ESP_LOGE(TAG, "Failed to find LittleFS partition");
    } else {
      ESP_LOGE(TAG, "Failed to initialize LittleFS (%s)", esp_err_to_name(ret));
    }
  }

  return ret;
}

void write_string_to_file() {
  FILE *f = fopen(FILENAME, "w");
  if (f == NULL) {
    ESP_LOGE(TAG, "Failed to open file for writing");
    return;
  }

  fprintf(f, "%s", DATA);
  fclose(f);

  ESP_LOGI(TAG, "String written to file: %s", DATA);
}

void read_string_from_file() {
  FILE *f = fopen(FILENAME, "r");
  if (f == NULL) {
    ESP_LOGE(TAG, "Failed to open file for reading");
    return;
  }

  char buffer[32];  // Suponha que a string tem no máximo 32 caracteres
  if (fgets(buffer, sizeof(buffer), f) != NULL) {
    ESP_LOGI(TAG, "Read string from file: %s", buffer);
  } else {
    ESP_LOGE(TAG, "Failed to read from file");
  }

  fclose(f);
}

void app_main() {
  gpio_set_direction(GPIO_NUM_45, GPIO_MODE_OUTPUT);
  uart_config_t uart_config = {
      .baud_rate = 115200,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
  };

  uart_param_config(UART_NUM_0, &uart_config);
  uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

  uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);

  while (!uart_is_driver_installed(UART_NUM_0)) {
    vTaskDelay(pdMS_TO_TICKS(50));
  }

  println("init");

  gpio_set_level(GPIO_NUM_45, 1);
  vTaskDelay(pdMS_TO_TICKS(20));
  gpio_set_level(GPIO_NUM_45, 0);

  init_littlefs();

  write_string_to_file();
  read_string_from_file();

  gpio_set_level(GPIO_NUM_45, 1);
  vTaskDelay(pdMS_TO_TICKS(20));
  gpio_set_level(GPIO_NUM_45, 0);

  while (1) {

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}