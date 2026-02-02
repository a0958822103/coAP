#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <coap-simple.h>

// --- 網路設定 ---
const char* ssid     = "test_AP";
const char* password = "passwordis123";

// --- Raspberry Pi 設定 ---
const char* server_ip = "192.168.4.1"; 
const int server_port = 5683;
const char* endpoint  = "battery";       // 對應 Pi 上的 root.add_resource(['battery'], ...)

// UDP 與 CoAP 實例
WiFiUDP udp;
Coap coap(udp);

// 當伺服器回傳訊息時觸發（Callback）
void callback_response(CoapPacket &packet, IPAddress ip, int port) {
    char p[packet.payloadlen + 1];
    memcpy(p, packet.payload, packet.payloadlen);
    p[packet.payloadlen] = '\0';
    
}

void setup() {
    Serial.begin(115200);
    delay(1000);    

    // 1. 連接 WiFi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("正在連接 WiFi...");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi 已連線！");
    Serial.print("ESP32 IP: ");
    Serial.println(WiFi.localIP());

    // 2. 註冊回應回呼函式
    coap.response(callback_response);

    // 3. 啟動 CoAP 客戶端
    coap.start();
    Serial.println("CoAP 客戶端已啟動...");
}

void loop() {
    // 1. 模擬電池數據
    float simulated_voltage = random(370, 420) / 100.0;
    String payload = String(simulated_voltage) + "V";

    // 2. 先將字串 IP 轉換為 IPAddress 物件
    IPAddress target_ip;
    if (target_ip.fromString(server_ip)) {
        
        Serial.print("發送電池數據至 Pi: ");
        Serial.println(payload);

        uint16_t msgid = coap.put(target_ip, server_port, endpoint, payload.c_str());
        // -----------------------

    } else {
        Serial.println("錯誤：無效的伺服器 IP 位址");
    }

    coap.loop();
    delay(5000);
}