#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define FIXED_TEXT_HEIGHT 16 // 固定区域高度
#define SCROLL_START_Y (FIXED_TEXT_HEIGHT) // Start of scrollable area
#define LINE_HEIGHT 8 // Adjust based on your font size

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

String receivedText = "";
bool newData = false;
unsigned long lastScrollTime = 0;
int scrollY = 0; // Y offset for scrolling text
const unsigned long SCROLL_DELAY = 5000; // Time delay between scrolls in ms
bool isScrolling = false;
int textHeight = 0; // Declare textHeight globally
int currentBlockIndex = 0;
String blocks[10]; // Assuming max 10 blocks, adjust as needed
int blockCount = 0;

void setup() {
  Serial.begin(115200);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("To Do List");
  display.display();
}

void loop() {
  // 读取串口输入
  while (Serial.available()) {
    char receivedChar = Serial.read();

    // 有*则清空
    if (receivedChar == '*'){
      receivedText ="";
      break;
    }

    // 有回车则开始导入结果
    if (receivedChar == '\r') {
      newData = true;
      break;
    } else {
      receivedText += receivedChar;
    }
  }

  // 如果有回车输入
  if (newData) {
    splitTextIntoBlocks(receivedText);
    displayCurrentBlock();
    newData = false;
    lastScrollTime = millis(); // Reset the timer when new data arrives
    isScrolling = true;
  }

  // 处理滚动逻辑
  if (isScrolling && millis() - lastScrollTime > SCROLL_DELAY) {
    lastScrollTime = millis();
    currentBlockIndex = (currentBlockIndex + 1) % blockCount;
    displayCurrentBlock();
  }
}

void clearScrollArea() {
  display.fillRect(0, 16, 128, 48, SSD1306_BLACK);
  display.display();
}

void drawFixedText() {
  // Redraw fixed text to ensure it remains visible
  display.setCursor(0, 0);
  display.print("To Do List");
  display.display();
}

void splitTextIntoBlocks(String text) {
  int startIndex = 0;
  int endIndex = 0;
  blockCount = 0;

  while ((endIndex = text.indexOf('|', startIndex)) != -1) {
    String tempBlock = text.substring(startIndex, endIndex);
    tempBlock.trim();
    blocks[blockCount++] = tempBlock;
    startIndex = endIndex + 1;
  }
  String tempBlock = text.substring(startIndex);
  tempBlock.trim();
  blocks[blockCount++] = tempBlock;
}

void displayCurrentBlock() {
  clearScrollArea();
  drawFixedText();

  display.setCursor(0, SCROLL_START_Y);
  display.setTextSize(2);
  display.print(blocks[currentBlockIndex]);
  display.display();
}
