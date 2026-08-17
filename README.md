# Magnetic Levitation System 

An automated magnetic levitation system that stabilizes a ferromagnetic object in mid-air using active electromagnetic control, Hall Effect / Infrared feedback sensors, and closed-loop PID control algorithms.

---

## 📌 Project Overview 

This project focuses on designing and implementing a dynamic magnetic levitation system. By measuring the position of a floating object via feedback sensors, the controller dynamically adjusts the current passing through an electromagnet to maintain a stable levitation distance.

### Key Features
- **Closed-Loop Control:** Real-time PID feedback control algorithm running on Arduino.
- **Position Sensing:** Infrared / Hall-Effect sensor array for precise position tracking.
- **Power Electronics:** Transistor/MOSFET driver circuit for fast PWM current switching.
- **Data Visualization:** Real-time sensor and control signal tracking via Arduino Serial Plotter / MATLAB.

---

## 🛠 Hardware Architecture 

- **Microcontroller:** Arduino Uno (Atmega328P)
- **Actuator:** Custom-wound Electromagnetic Coil
- **Sensor:** Hall Effect Sensor / Infrared Transceiver Pair
- **Power Driver:** MOSFET / Power Transistor Driving Circuit + Flyback Diode Protection
- **Power Supply:** External DC Power Source

---

## 💻 Control & Software

The system computes the position error ($\text{Error} = \text{SetPoint} - \text{CurrentPosition}$) at a high frequency and outputs a corrected PWM signal to the power driver.
