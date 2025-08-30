# Stopwatch with Dual Mode (Increment and Countdown) ⏱️

## 📌 Project Overview

This project implements a **digital stopwatch** using an **ATmega32 microcontroller** and **six multiplexed seven-segment displays (common anode)**. The stopwatch operates in two modes:

* **Increment Mode (Default):** Counts up from `00:00:00`.
* **Countdown Mode:** Counts down from a user-defined time until `00:00:00`, triggering a buzzer when time is up.

---

## 🎯 Objective

To design and implement a **dual-mode stopwatch** with **reset, pause, resume, and mode toggle** functionality, providing accurate time tracking and clear visual indicators.

---

## ⚙️ Features

* **Increment Mode**: Starts counting up automatically.
* **Countdown Mode**: User can set hours, minutes, and seconds before starting the countdown.
* **Reset**: Resets time to zero.
* **Pause/Resume**: Temporarily stops or resumes counting.
* **LED Indicators**:

  * Red LED → Increment mode
  * Yellow LED → Countdown mode
* **Alarm/Buzzer**: Alerts when countdown reaches zero.

---

## 🛠️ Hardware Requirements

* **Microcontroller**: ATmega32 (16 MHz system clock)
* **Displays**: Six common-anode 7-segment displays with a **7447 BCD to 7-segment decoder**
* **Push Buttons**:

  * Reset (PD2 - INT0)
  * Pause (PD3 - INT1)
  * Resume (PB2 - INT2)
  * Mode Toggle (PB7)
  * Hour, Minute, and Second adjustments (PB0–PB6)
* **Alarm**: Buzzer on PD0
* **LEDs**:

  * PD4 → Increment Mode LED
  * PD5 → Countdown Mode LED

---

## 🧠 Software Requirements

* **Compiler/IDE**: Code Composer Studio (CCS) or AVR Studio
* **Programming Language**: C (with FreeRTOS support)
* **Timer Configuration**:

  * **Timer1 in CTC mode** for precise timing
* **Interrupts**:

  * INT0 → Reset
  * INT1 → Pause
  * INT2 → Resume

---

## 🔧 How It Works

### **Increment Mode** (Default)

1. Power up the system → Stopwatch starts counting up.
2. Red LED (PD4) turns ON.

### **Countdown Mode**

1. Pause the stopwatch using the **Pause Button**.
2. Toggle to countdown mode using **PB7**.
3. Set the desired countdown time using the hour, minute, and second adjustment buttons.
4. Press **Resume Button (PB2)** to start countdown.
5. When countdown hits zero, buzzer is activated, and yellow LED (PD5) indicates countdown mode.

---

### 🔗 Demo
Check the project details on LinkedIn:  
[https://www.linkedin.com/posts/mahmoud-abouzeid-sw-ml_embeddedsystems-atmega32-microcontrollers-activity-7367664010571214849-sb6G](https://www.linkedin.com/posts/mahmoud-abouzeid-sw-ml_embeddedsystems-atmega32-microcontrollers-activity-7367664010571214849-sb6G)


---

## 📝 Authors

**Edges For Training Team**
