#include <LiquidCrystal.h>
#include <WiFi.h>
#include <qrcode.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

float totalCost = 0.0;
String scannedItems = "";
String lastBarcode = "";
int quantity = 0;
const int incrementButtonPin = 7;
const int decrementButtonPin = 8;
const int checkoutButtonPin = 9;
bool checkout = false;

const char* ssid = "Mywifi";
const char* password = "1234567890";

QRCode qrcode;

void setup() {
    lcd.begin(16, 2);
    lcd.print("Welcome to Smart Cart");
    lcd.setCursor(0, 1);
    lcd.print("Billing System");
    
    pinMode(incrementButtonPin, INPUT_PULLUP);
    pinMode(decrementButtonPin, INPUT_PULLUP);
    pinMode(checkoutButtonPin, INPUT_PULLUP);
    
    Serial.begin(9600);
    
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
}

void loop() {
    if (Serial.available()) {
        String barcode = Serial.readStringUntil('\n');
        float price = getPriceFromBarcode(barcode);
        
        if (price > 0) {
            lastBarcode = barcode;
            quantity = 1;
            totalCost += price;
            scannedItems += barcode + " x" + String(quantity) + " ($" + String(price) + ")\n";
            updateDisplay();
        } else {
            lcd.clear();
            lcd.print("Item Not Found");
            delay(2000);
        }
    }

    if (digitalRead(incrementButtonPin) == LOW) {
        incrementQuantity();
        delay(200);
    }

    if (digitalRead(decrementButtonPin) == LOW) {
        decrementQuantity();
        delay(200);
    }

    if (digitalRead(checkoutButtonPin) == LOW) {
        checkout = true;
        finalizeCheckout();
    }
}

float getPriceFromBarcode(String barcode) {
    if (barcode == "123456") return 10.99;
    if (barcode == "654321") return 5.49;
    return 0;
}

void updateDisplay() {
    lcd.clear();
    lcd.print("Total: $" + String(totalCost));
    lcd.setCursor(0, 1);
    lcd.print(scannedItems);
}

void incrementQuantity() {
    if (quantity < 99) {
        quantity++;
        totalCost += getPriceFromBarcode(lastBarcode);
        updateDisplay();
    }
}

void decrementQuantity() {
    if (quantity > 1) {
        quantity--;
        totalCost -= getPriceFromBarcode(lastBarcode);
        updateDisplay();
    }
}

void finalizeCheckout() {
    lcd.clear();
    lcd.print("Total: $" + String(totalCost));
    lcd.setCursor(0, 1);
    lcd.print("Checkout...");
    
    String paymentLink = "https://paymentgateway.com/pay?amount=" + String(totalCost);
    generateQRCode(paymentLink);
    
    delay(5000);
    resetCart();
}

void generateQRCode(String data) {
    qrcode.init();
    qrcode.addData(data);
    qrcode.make();

    lcd.clear();
    lcd.print("Pay at: ");
    lcd.setCursor(0, 1);
    lcd.print(data);
    delay(1000);
}

void resetCart() {
    totalCost = 0.0;
    scannedItems = "";
    quantity = 0;
    lastBarcode = "";
    lcd.clear();
    lcd.print("Welcome to Smart Cart");
    lcd.setCursor(0, 1);
    lcd.print("Billing System");
}
