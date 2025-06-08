# 🏀 ABU Robocon 2025 – HEROES Team Codebase (Basketball Challenge)

Welcome to the official code repository of **Team HEROES**, representing **Universitas Gadjah Mada** in the **ABU Robocon Indonesia 2025** competition. This year’s theme, *“Team Effort, Ultimate Success,”* challenges us to design two basketball-playing robots capable of navigating, shooting, and collaborating like true champs!

---

## 📌 Team Information

- **Team Name**: HEROES  
- **University**: Universitas Gadjah Mada, Yogyakarta  
- **Supervisor**: Lukman Awaludin, S.Si., M.Cs.  
- **Team Leader**: Muhammad Farhan Rayhansyah  
- **Email**: muhammadfarhanrayhan@mail.ugm.ac.id

### 👥 Team Members:
- Aufa Rafidya Setfiano  
- Heikmal Imana Mutawali  
- Ihsan Hammam  
- Ridwan (and more...)

---

## ⚙️ Robot Overview

We are building **two manual robots** for the game:

### 🤖 Robot R1
- **Role**: Ball pickup, storage, and shooting  
- **Size (Initial)**: 800 x 800 x 120 mm  
- **Size (Extended)**: 800 x 1300 x 130 mm  
- **Wheels**: Omni-wheels  
- **Actuators**: DC Motor with planetary gear, DC 895 motor, pneumatic and electric actuators  
- **Material**: Aluminium profile and plates  
- **Mechanisms**:
  - Gripper
  - Conveyor
  - Ball Launcher

### 🤖 Robot R2
- **Role**: Ball passing, supporting R1  
- **Size (Both states)**: 800 x 800 x 138 mm  
- **Mechanisms**:
  - Gripper
  - Ball Launcher
- **Actuators**: Planetary gear motors and pneumatic systems  
- **Material**: Similar aluminium-based frame  

---

## 🧠 Control Systems

- **Both Robots**: Manual control via wireless transmitter  
- **Controllers**: ESP32 v4
- **Sensor Modules**: In development (likely to include encoders, IR, and camera systems)

---

## 💡 Strategy & Algorithm

Our gameplay involves:
- Coordinated movement between R1 and R2
- Strategic ball pickup and shooting zones
- Synchronized passing and scoring sequences
- Positioning based on game dynamics

We use **modular state machines** and **event-driven logic** for precise timing and execution.

---

