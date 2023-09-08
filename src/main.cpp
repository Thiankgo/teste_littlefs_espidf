#include <stdio.h>

#include <string>

#include "driver/gpio.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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
  while (init_littlefs() != ESP_OK) {
    vTaskDelay(pdMS_TO_TICKS(100));
  }


  while (1) {
    write_string_to_file();
    read_string_from_file();

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}