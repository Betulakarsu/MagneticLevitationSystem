# Magnetic Levitation System

An automated magnetic levitation system that stabilizes a ferromagnetic object in mid-air using dynamic electromagnetic control, position feedback sensors, and real-time closed-loop PID control algorithms.

---

## 📌 Project Overview

This project focuses on the modeling, hardware implementation, and control of an inherently unstable dynamic magnetic levitation system. By measuring the real-time vertical position of a suspended object, the controller dynamically adjusts the pulse-width modulation (PWM) signal fed to an electromagnet driver to balance gravitational and magnetic forces.

### Key Features
* **Closed-Loop Control:** Real-time digital PID controller running on an Arduino Uno microcontroller.
* **High-Precision Position Feedback:** Analog sensing using Hall-Effect / Infrared sensor pairs for sub-millimeter position tracking.
* **Power Switching Electronics:** High-speed MOSFET driver stage featuring flyback diode protection to dissipate inductive kickback.
* **Non-Linear Dynamics Control:** Linearization of the electromagnetic force-distance relationship around an equilibrium setpoint.
* **Real-Time Telemetry:** Live feedback visualizer via Arduino Serial Plotter / MATLAB for PID tuning and step-response analysis.

---

## ⚙️ Mathematical Model & Control Logic

The system balances gravitational force against the non-linear electromagnetic force:

$$F_{\text{net}} = m \cdot g - F_{\text{mag}}(I, x)$$

Where the magnetic levitation force is governed by:

$$F_{\text{mag}}(I, x) = C \cdot \left( \frac{I}{x} \right)^2$$

* $I$: Current passing through the electromagnet coil
* $x$: Vertical position / distance from coil face
* $C$: Electromagnetic force constant

### PID Control Algorithm
The controller calculates position error at high sampling frequencies:

$$\text{Error}(t) = \text{SetPoint} - \text{CurrentPosition}(t)$$

$$\text{Output}(t) = K_p \cdot e(t) + K_i \int e(t) \, dt + K_d \cdot \frac{de(t)}{dt}$$

The continuous control output is mapped to an 8-bit PWM duty cycle driving the MOSFET gate.

---
## 🛠 Hardware Architecture
<img width="1111" height="737" alt="harness_diagram png" src="https://github.com/user-attachments/assets/4ec7d8fd-606c-47c0-976f-406d91089e88" />

| Component | Specification | Description |
| :--- | :--- | :--- |
| **Microcontroller** | ATmega328P (Arduino Uno) | Main digital signal processor executing the control loop |
| **Actuator** | Custom Electromagnet Coil | Inductive element generating variable attractive force |
| **Sensor Stage** | Hall Effect / IR Receiver Pair | Analog positional feedback tracking the levitation gap |
| **Power Stage** | N-Channel Logic Level MOSFET | PWM switching transistor (e.g., IRLZ44N / IRF540N) |
| **Protection** | Fast Recovery Diode | Flyback diode parallel to coil for inductive surge protection |
| **Power Supply** | External DC Power Source | Dedicated power line for coil current demand |


## 📐 System Block Diagram


       +------------+        +---------------+        +--------------+
       |  SetPoint  | -----> |  Arduino Uno  | -----> | MOSFET Driver|
       +------------+        |  (PID Loop)   |        |   Circuit    |
             ^               +---------------+        +--------------+
             |                       |                       |
             |                       v                       v
       +------------+        +---------------+        +--------------+
       | Feedback   | <----- | Floating Object|<----- | Electromagnet|
       | Sensor     |        | (Position x)  |        |  (Coil Current)
       +------------+        +---------------+        +--------------+
---
<img width="751" height="180" alt="simulink_block_diagram png" src="https://github.com/user-attachments/assets/d9daec68-3eff-4acb-a4b4-3b1e2d9ffc5f" />

## 📁 Repository Structure
```text
MagneticLevitation/
├── src/
│   └── maglev_controller.ino    # Core PID control algorithm source code
├── hardware/
│   ├── circuit_diagram.png      # Schematic diagram
│   └── simulation.pdsprj        # Circuit simulation file
├── .gitignore                   # Version control exclusions
└── README.md                    # Project documentation
