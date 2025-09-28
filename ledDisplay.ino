#include <MD_Parola.h>    // Library for controlling text animations on LED matrix displays
#include <MD_MAX72xx.h>   // Core library for controlling MAX7219/MAX7221 LED drivers
#include <SPI.h>          // SPI communication library (used by MAX72xx modules)

// Define the type of hardware and connection pins
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW   // Hardware type: FC-16 module (commonly used 8x8 LED matrix)
#define MAX_DEVICES 4                      // Number of LED matrices chained together
#define CLK_PIN 13                         // Clock pin for SPI
#define DATA_PIN 11                        // Data pin for SPI
#define CS_PIN 10                          // Chip Select (Load) pin

// Create display objects
MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
// myDisplay → Used for text animations
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
// mx → Used for direct pixel/column control (custom shapes, drawings)

bool stopAnimation = false;   // Flag to stop text animation when showing a custom shape

#define BUF_Size 100          // Buffer size for incoming serial data
char inputChar[BUF_Size];     // Array to store the message received from Serial
byte index = 0;               // Index tracker for buffer (not actively used here)

void setup() {
  Serial.begin(115200);       // Initialize Serial communication at 115200 baud rate
  myDisplay.begin();          // Initialize the Parola display
  myDisplay.setIntensity(4);  // Set brightness level (0–15)
  myDisplay.displayClear();   // Clear any content on the display
  mx.begin();                 // Initialize MAX72xx object (for custom drawing)
  // myDisplay.displayText("Hello EFS", PA_CENTER, 80, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  // Example to show scrolling text (commented out for now)
}

// Define a custom arrow shape (8x8 bitmap stored in binary)
byte arrowRight[8] = {
  B10011000,
  B00011100,
  B00011110,
  B11111111,
  B11111111,
  B00011110,
  B00011100,
  B00011000
};

// Function to draw a shape on a specific LED matrix device
void drawShape(int device, byte shape[8]) {
  for (int col = 0; col < 8; col++) {
    mx.setColumn(device, col, shape[col]);  // Set each column of the matrix from the shape array
  }
}

void loop() {
  // Check if Serial data is available
  while (Serial.available() > 0) {
    String str = Serial.readStringUntil('\n');  // Read input string until newline character

    if (str == "arrowRight") {   // If the received command is "arrowRight"
      myDisplay.displayClear();  // Clear text display
      memset(inputChar, 0, BUF_Size); // Reset input buffer
      stopAnimation = true;      // Stop text animation
      drawShape(1, arrowRight);  // Draw arrow shape on device #1 (second module in chain)
    } 
    else {  // Otherwise, treat the input as text to scroll
      stopAnimation = false;
      str.toCharArray(inputChar, BUF_Size);   // Convert string into char array
      myDisplay.displayClear();               // Clear previous display
      // Display text with Parola: center aligned, scroll speed 80ms, no pause,
      // scrolling left in and left out
      myDisplay.displayText(inputChar, PA_CENTER, 80, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    }
  }

  // If animation is enabled, run the animation
  if (!stopAnimation && myDisplay.displayAnimate()) {
    myDisplay.displayReset();   // Reset animation so text loops continuously
  }
}
