/**
 * @file mouse-jiggler.ino
 * @author being24
 * @brief Sketch for making a mouse jiggler that works randomly with sparkfun
 * pro micro
 * @version 0.1
 * @date 2023-03-31
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "HID-Project.h"

// Variable that stores the current position of the mouse pointer
int current_x = 0;
int current_y = 0;

// Variable that stores the coordinates of the destination of the mouse pointer
int destination_x = 0;
int destination_y = 0;

// Variable that stores whether to stop moving the mouse pointer
bool move = true;

// Create an array of behavior choices
String options[] = {"straight", "stop"};

// Define maximum and minimum x, y coordinates
int xMin = -16384;
int xMax = 16384;
int yMin = -16384;
int yMax = 16384;

// Defines the range of x and y coordinates for moving the mouse pointer from
// the edge back to the center.
int center_xMin = -1000;
int center_xMax = 1000;
int center_yMin = -1000;
int center_yMax = 1000;

void setup() {
  // Set TX and RX pins as inputs to suppress blinking during serial
  // communication
  pinMode(LED_BUILTIN_TX, INPUT);
  pinMode(LED_BUILTIN_RX, INPUT);

  // Open Serial Monitor
  Serial.begin(9600);

  // Wait 1000 milliseconds
  delay(1000);

  // Initialize the AbsoluteMouse
  AbsoluteMouse.begin();

  // Move the mouse pointer to the center of the screen
  AbsoluteMouse.moveTo(0, 0);
}

int randMs(int minMs, int maxMs) {
  /**
   * @brief Function to return a random number of milliseconds between minMs and
   * maxMs
   *
   */
  if (minMs < 0.0) {
    minMs = 0.0;
  }
  if (minMs == maxMs) {
    maxMs = maxMs + 1;
  }
  return random(int(minMs), int(maxMs));
}

void delayRandomMs(int Ms) {
  /**
   * @brief Random delay function between Ms±500 ms
   *
   */
  delay(randMs(Ms - 500, Ms + 500));
}

void moveMouseStraight(float fromX, float fromY, float toX, float toY,
                       int moveMs) {
  /**
   * @brief Function to moveMs milliseconds from fromX, fromY to toX, toY
   *
   */

  // Number of mouse pointer trajectory divisions
  // If this number is increased, it moves more smoothly.
  // However, due to the library's specification, if you make it too fine and
  // try to move to the same coordinate more than once, it will cause a problem.
  int moveCount = 200;

  // Unit amount of mouse pointer movement
  float moveX = (toX - fromX) / moveCount;
  float moveY = (toY - fromY) / moveCount;

  // unit of time
  float moveTime = moveMs / moveCount;

  // Move the mouse pointer
  for (int i = 0; i < moveCount; i++) {
    if (move == true) {
      AbsoluteMouse.moveTo(fromX + moveX * i, fromY + moveY * i);
    }
    delay(moveTime);
  }

  // Update the current coordinates of the mouse pointer
  current_x = toX;
  current_y = toY;
}

void loop() {
  // Move to coordinate (16bit signed, -32768 - 32767)
  // Moving to the same position twice will not work!
  // X and Y start in the upper left corner.

  // Display current coordinates
  Serial.println("X: " + String(current_x));
  Serial.println("Y: " + String(current_y));

  // Randomly determines the time for the mouse pointer to move within a given
  // range
  float moveMs = randMs(500, 4000);
  Serial.println("Move: " + String(moveMs) + " millis");

  // If current_x,y is outside the xMin,xMax,yMin,yMax range, move to somewhere
  // within the defined center range and return the mouse pointer to center
  if (current_x < xMin || current_x > xMax || current_y < yMin ||
      current_y > yMax) {
    Serial.println("centering");

    destination_x = random(center_xMin, center_xMax);
    destination_y = random(center_yMin, center_yMax);
    moveMouseStraight(current_x, current_y, destination_x, destination_y,
                      moveMs);
  }

  // Randomly select a behavior from options
  String option = options[random(0, 2)];
  Serial.println(option);

  // If the option is "stop", stop moving the mouse pointer
  if (option == "stop") {
    delayRandomMs(moveMs);
    return;
  }

  // If the option is "straight", move the mouse pointer to a random position
  if (option == "straight") {
    // 移動先の座標を決定する
    destination_x = random(xMin, xMax);
    destination_y = random(yMin, yMax);

    moveMouseStraight(current_x, current_y, destination_x, destination_y,
                      moveMs);
    return;
  }
}