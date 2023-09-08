cmake_minimum_required(VERSION 3.16.0)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(teste_littlefs_espidf)
littlefs_create_partition_image(littlefs ../flash_data FLASH_IN_PROJECT)
