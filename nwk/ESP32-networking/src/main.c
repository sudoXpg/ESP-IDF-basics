#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_log.h"



#define MAX_STATIONS 10

int get_wifi_ids();

void app_main(void){
    esp_log_level_set("*", ESP_LOG_NONE);
    nvs_flash_init();
    while(1){
        printf("Starting Wi-Fi scan...\n");
        get_wifi_ids();
    
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}



int get_wifi_ids(){
    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    esp_err_t ret = esp_wifi_init(&config);
    if(ret!=ESP_OK){
        printf("Wi-Fi initialization failed with error: %d\n", ret);
        return ret;
    }

    ret = esp_wifi_set_mode(WIFI_MODE_STA);
    if(ret!=ESP_OK){
        printf("Failed to set Wi-Fi mode with error: %d\n", ret);
        return ret;
    }
    
    ret = esp_wifi_start();
    if(ret!=ESP_OK){
        printf("Failed to start Wi-Fi with error: %d\n", ret);
        return ret;
    }

    wifi_scan_config_t scan_config = {
    .ssid = NULL,                           // NULL means scan all SSIDs
    .bssid = NULL,                          // NULL means scan all BSSIDs
    .channel = 0,                           // Scan all channels
    .show_hidden = true,                    // Show hidden networks
    .scan_time.active.min = 500,            // Adjust scan time (in ms)
    .scan_time.active.max = 800
    };



    ret = esp_wifi_scan_start(&scan_config, true);
    if(ret!=ESP_OK){
        printf("Wi-Fi scan start failed with error: %d\n", ret);
        return ret;
    }
    uint16_t count = MAX_STATIONS;
    wifi_ap_record_t *ap_records=malloc(sizeof(wifi_ap_record_t)*MAX_STATIONS);
    if(!ap_records){
        printf("Memory allocation for AP records failed\n");
        return ESP_ERR_NO_MEM;
    }
    
    ret = esp_wifi_scan_get_ap_records(&count, ap_records);
    if (ret != ESP_OK) {
        printf("Failed to get AP records with error: %d\n", ret);
        free(ap_records);
        return ret;
    }
    
    
    printf("-------------------------------\n");
    
    for(int i=0;i<count;i++){
        printf("SSID : %s  || Signal Strength : %d \n",ap_records[i].ssid,ap_records[i].rssi);
    }
    
    free(ap_records);
    esp_wifi_stop();
    esp_wifi_deinit();


    return ret;
}