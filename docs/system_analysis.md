# System Analysis & Control Validation

This document provides a detailed breakdown of the mathematical modeling, Simulink closed-loop architecture, and real-time hardware performance of the Magnetic Levitation System.

---

## 1. Mathematical Model & Simulink Block Diagram

The linearized transfer function of the magnetic levitation plant $G(s)$ is modeled as:

$$G(s) = \frac{K}{s^2 - a^2}$$

To analyze the system stability and tune the Discrete PID parameters, the following closed-loop architecture was constructed in MATLAB/Simulink:

<img width="751" height="180" alt="simulink_block_diagram png" src="https://github.com/user-attachments/assets/c085c22e-bda7-4260-a897-30c16aba9bd1" />

### Model Components
- **`PID(z)`:** Discrete PID controller simulating the embedded control loop executing on the microcontroller.
- **`-K-` (Actuator Gain):** Represents the driver circuit and PWM voltage conversion factor.
- **`Sys_MagLev`:** Linearized continuous/discrete transfer function of the electromagnet and floating object system.
- **`-K-` (Sensor Gain):** Position sensor calibration factor mapping distance to analog feedback values.

---

## 2. Real-Time Hardware Validation (Arduino Serial Plotter)

After tuning the parameters theoretically, the control loop was deployed to the microcontroller. The real-time sensor feedback and control signals were captured during physical levitation tests:


<img width="917" height="496" alt="arduino_serial_plotter png" src="https://github.com/user-attachments/assets/66b83d3e-f887-48c2-81c2-fc7fd0893bfe" />

### Key Observations
- The real-time position signal (blue line) tracks the reference setpoint (red line) during dynamic disturbance recovery.
- The control action adjusts dynamically to prevent coil overheating and maintain stable hovering distance.
