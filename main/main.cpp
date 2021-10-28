/* Simple HTTP Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "esp_timer.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_netif_ppp.h"
#include "esp_eth.h"
#include "protocol_examples_common.h"
#include <string.h>
#include "ESPAsyncWebServer.h"
#include "cJSON.h"


// #include <esp_http_server.h>

/* A simple example that demonstrates how to create GET and POST
 * handlers for the web server.
 */

static const char *TAG = "example";

/* 
 * Load the stub html, a loader that preferentially loads from localhost for debugging
 */
// static esp_err_t stub_handler(httpd_req_t *req)
// {
//     httpd_resp_set_type(req, "text/html");
//     httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "http://127.0.0.1");

//     /* Get handle to embedded file upload script */
//     extern const unsigned char stub_start[] asm("_binary_stub_html_start");
//     extern const unsigned char stub_end[]   asm("_binary_stub_html_end");
//     const size_t stub_size = (stub_end - stub_start);

//     /* Add file upload form and script which on execution sends a POST request to /upload */
//     httpd_resp_send_chunk(req, (const char *)stub_start, stub_size);

//     /* Send empty chunk to signal HTTP response completion */
//     httpd_resp_sendstr_chunk(req, NULL);

//     return ESP_OK;
// }

// static const httpd_uri_t stub_html = {
//     .uri       = "/",
//     .method    = HTTP_GET,
//     .handler   = stub_handler,
//     .user_ctx  = NULL
// };

// /* get some info */
// static esp_err_t info_get_handler(httpd_req_t *req)
// {
//     char buf[100];
//     char ipbuf[20];
//     esp_netif_ip_info_t ip_info = {0};
//     esp_netif_get_ip_info(get_example_netif(), &ip_info);
//     esp_ip4addr_ntoa(&ip_info.ip, ipbuf, sizeof(ipbuf));
//     snprintf(buf, sizeof(buf), "Time since boot: %f\nip_addr: %s", (float)esp_timer_get_time()/1000000, ipbuf);
//     httpd_resp_sendstr(req, buf);
//     return ESP_OK;
// }

// static const httpd_uri_t info = {
//     .uri       = "/info",
//     .method    = HTTP_GET,
//     .handler   = info_get_handler,
//     .user_ctx  = NULL
// };

AsyncWebServer server(80);
AsyncEventSource events("/events");


extern "C" void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    // httpd_handle_t* server = (httpd_handle_t*) arg;
    // if (*server) {
    //     ESP_LOGI(TAG, "Stopping webserver");
    //     stop_webserver(*server);
    //     *server = NULL;
    // }
}

void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    ESP_LOGI(TAG, "Starting server");

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        // request->send(200, "text/html", "Hello, world");

        /* Get handle to embedded file upload script */
        extern const unsigned char stub_start[] asm("_binary_stub_html_start");
        extern const unsigned char stub_end[]   asm("_binary_stub_html_end");
        const size_t stub_size = (stub_end - stub_start);

        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", stub_start, stub_size);
        response->addHeader("Server","ESP Async Web Server");
        request->send(response);
    });

    events.onConnect([](AsyncEventSourceClient *client){
        if(client->lastId()){
            ESP_LOGI(TAG, "Client reconnected! Last message ID that it gat is: %u\n", client->lastId());
        }
        //send event with message "hello!", id 999
        // and set reconnect delay to 1 second
        client->send("hello!",NULL,999 ,1000);

    });

    // events.setAuthentication("user", "pass");

    server.addHandler(&events);
    server.begin();
    ESP_LOGI(TAG, "Started server");
    // httpd_handle_t* server = (httpd_handle_t*) arg;
    // if (*server == NULL) {
    //     ESP_LOGI(TAG, "Starting webserver");
    //     *server = start_webserver();
    // }
}


extern "C" void app_main(void)
{
    // static httpd_handle_t server = NULL;

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, NULL));

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

//   void handleRequest(AsyncWebServerRequest *request) {
//     AsyncResponseStream *response = request->beginResponseStream("text/html");
//     response->print("<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>");
//     response->print("<p>This is out captive portal front page.</p>");
//     response->printf("<p>You were trying to reach: http://%s%s</p>", request->host().c_str(), request->url().c_str());
//     response->printf("<p>Try opening <a href='http://%s'>this link</a> instead</p>", WiFi.softAPIP().toString().c_str());
//     response->print("</body></html>");
//     request->send(response);
//   }

    while (true) {
            //send event "myevent"
        events.send("my event content","myevent",esp_timer_get_time());
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
