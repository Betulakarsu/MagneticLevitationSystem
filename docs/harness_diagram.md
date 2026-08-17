# Harness & Wiring Diagram (Kablo Bağlantı Şeması)

This document details the physical pinout, signal routing, and harness wiring for the Magnetic Levitation System.

---

## 🔌 Pin Mapping Table (Pin Bağlantı Tablosu)

| Component (Bileşen) | Module Pin | Arduino Pin | Wire Color (Kablo Rengi) | Notes / Signal Type |
| :--- | :--- | :--- | :--- | :--- |
| **Hall / IR Sensor** | VCC | 5V | Red (Kırmızı) | Power Supply |
| **Hall / IR Sensor** | GND | GND | Black (Siyah) | Ground |
| **Hall / IR Sensor** | OUT / Signal | A0 | Yellow (Sarı) | Analog Position Signal |
| **MOSFET Driver** | Gate (IN) | D9 | White (Beyaz) | PWM Control Signal |
| **MOSFET Driver** | GND | GND | Black (Siyah) | Common Ground |
| **Electromagnet** | Coil (+) | External VCC | Red (Kırmızı) | 12V / High Current Power |
| **Electromagnet** | Coil (-) | MOSFET Drain | Blue (Mavi) | Switched Ground |

---

## ⚡ Circuit Protection & Notes

- **Flyback Diode:** Connected across the electromagnetic coil to protect the MOSFET driver from reverse inductive voltage spikes.
- **Common Ground:** All sensor, microcontroller, and external power supply grounds are tied to a single point to eliminate ground loops.

---

## 🖼 Harness Schematic Visual

*(Upload your harness diagram image to `docs/harness_diagram.png` to render below)*

![Harness Wiring Diagram](harness_diagram.png)
<img width="1111" height="737" alt="harness_diagram png" src="https://github.com/user-attachments/assets/76ebb80f-cbeb-46fd-ae80-35eacd12056b" />
