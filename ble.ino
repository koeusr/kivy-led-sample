#include <M5StickCPlus.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// BLEサービスとキャラクタリスティックのUUID
#define SERVICE_UUID        "12345678-1234-5678-1234-56789abcdef0"
#define CHARACTERISTIC_UUID "87654321-4321-6789-4321-fedcba987654"

// グローバル変数
BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      M5.Lcd.println("Device connected");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      M5.Lcd.println("Device disconnected");
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string rxValue = pCharacteristic->getValue();
        if (rxValue.length() > 0) {
            M5.Lcd.print("Received Value: ");
            for (int i = 0; i < rxValue.length(); i++)
                M5.Lcd.print(rxValue[i]);

            M5.Lcd.println();

            // ここで受信したデータに基づいて何かを行う
            if (rxValue == "1") {
                digitalWrite(10, HIGH);
            } else {
                digitalWrite(10, LOW);
            }
        }
    }
};

void setup() {
    // M5StickCPlusの初期化
    M5.begin();
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextFont(2);
    M5.Lcd.setRotation(0);
    M5.Lcd.setCursor(0, 0);

    Serial.begin(115200);

    // MACアドレスの取得と表示
    uint8_t macBT[6]; // MACアドレス格納用配列を準備
    esp_read_mac(macBT, ESP_MAC_BT); // MACアドレス取得
    M5.Lcd.printf("%02X:%02X:%02X:%02X:%02X:%02X\r\n", macBT[0], macBT[1], macBT[2], macBT[3], macBT[4], macBT[5]);

    // 内蔵LEDのピンを出力に設定
    pinMode(10, OUTPUT);

    // BLEデバイスの初期化
    BLEDevice::init("M5StickCPlus");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // サービスの作成
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // キャラクタリスティックの作成
    pCharacteristic = pService->createCharacteristic(
                         CHARACTERISTIC_UUID,
                         BLECharacteristic::PROPERTY_READ |
                         BLECharacteristic::PROPERTY_WRITE
                       );

    pCharacteristic->setCallbacks(new MyCallbacks());
    pCharacteristic->setValue("0");

    // サービスの開始
    pService->start();

    // アドバタイジングの開始
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    M5.Lcd.println("Waiting for a client connection to notify...");
}

void loop() {
    // if (M5.BtnA.wasPressed()) 

    // デバイス接続状態の確認
    if (deviceConnected) {
        // ここで何かを行うことができます
    }
    delay(1000);
}