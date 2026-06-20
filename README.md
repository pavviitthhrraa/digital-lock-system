# Digital Lock System 🔐
### 8051 Microcontroller | Embedded C | Keil uVision | Proteus

> A fully functional password-based door lock system built on the 
> AT89S52 (8051) microcontroller — programmed in Embedded C, 
> simulated in Proteus, and deployed on real hardware!

![Team Photo](team.jpg)

---

## Overview

Security systems are at the heart of modern electronics. This project 
implements a complete **Digital Lock System** using the AT89S52 8051 
microcontroller on the Aryabhatta development board.

The system accepts a 4-digit password via a matrix keypad, displays 
status on a 16x2 LCD, controls a DC motor to physically lock/unlock a 
door, and triggers audio-visual alerts for wrong attempts. After 3 
consecutive wrong attempts, the system permanently locks itself — 
requiring a hardware reset to restore access.

Built as part of our Microcontroller course project — every peripheral 
was interfaced, tested in simulation, and verified on real hardware.

---

## Demo

> Real hardware implementation on Aryabhatta Board

![Hardware](hardware.jpg)

---

## Features

| Feature | Description |
|---------|-------------|
| Password Protection | 4-digit password, digits masked with * on LCD |
| Motor Door Control | DC gear motor via L293D — clockwise unlock, anticlockwise close |
| Auto Lock Security | Permanently locks after 3 wrong attempts |
| Audio Alert | Active buzzer sounds for 2 seconds on wrong entry |
| Visual Indicators | Green LED for correct, Red LED for wrong password |
| Smart Keypad | C=Clear, B=Backspace, #=Confirm, A×4=Admin Reset |
| LCD Status Display | Real-time status messages on 16×2 LCD |
| Attempt Counter | Live At:X/3 counter displayed on LCD |
| Active LOW Design | All outputs use current sinking mode — no transistors needed |

---

## Hardware Components

| Component | Purpose |
|-----------|---------|
| AT89S52 (8051) | Main microcontroller — Aryabhatta Board |
| 16×2 LCD (1602) | Display password prompts and status |
| 4×4 Matrix Keypad | Password input |
| L293D Motor Driver | Controls DC gear motor direction |
| 6V DC Gear Motor | Physical door lock/unlock mechanism |
| Active Buzzer | Audio alert on wrong password |
| Green LED | Correct password indicator |
| Red LED | Wrong password / lockout indicator |
| 10K Potentiometer | LCD contrast adjustment |
| 6V Battery | Motor power supply |

---

## Pin Configuration
P0.0 - P0.7  →  LCD Data Bus (D0-D7)

P1.0 - P1.3  →  Keypad Rows 1-4

P1.4 - P1.7  →  Keypad Columns 1-4

P2.0         →  LCD RS

P2.1         →  LCD EN

P3.1         →  Active Buzzer (Active LOW)

P3.2         →  L293D IN1

P3.3         →  L293D IN2

P3.6         →  Red LED (Active LOW)

P3.7         →  Green LED (Active LOW)

---

## Block Diagram

![Block Diagram](blockdiagram.jpg)

---

## System Flow
Power ON

↓

LCD shows "Pass: At:0/3"

↓

User enters 4-digit password via keypad

↓

├── Correct Password

│       ↓

│   Green LED ON

│   Motor unlocks door (3 seconds)

│   Motor auto-closes door

│   System resets → ready for next entry

│

└── Wrong Password

↓

Red LED ON + Buzzer 2 seconds

Attempt counter increments

↓

├── Attempts < 3 → Try again

│

└── Attempts = 3

↓

System LOCKED forever

Red LED blinks continuously

Only hardware RESET unlocks

---

## Keypad Functions

| Key | Function |
|-----|----------|
| 0-9 | Enter password digit |
| C | Clear all entered digits |
| B | Backspace (delete last digit) |
| # | Confirm password manually |
| A×4 | Secret admin reset (press A 4 times) |

---

## Tools Used

| Tool | Purpose |
|------|---------|
| Keil uVision | Embedded C IDE — code writing and compilation |
| ProgISP | HEX file flashing via USB programmer |
| Proteus | Circuit simulation before hardware build |
| Aryabhatta Board | AT89S52 development board |

---

## How to Run

### Software Simulation:
1. Open Proteus
2. Load the circuit schematic
3. Load the compiled `.hex` file
4. Run simulation and test all features

### Hardware Deployment:
1. Compile `digital_lock.c` in Keil uVision
2. Generate `.hex` file
3. Flash to AT89S52 using ProgISP
4. Connect all peripherals as per pin configuration
5. Power on — system ready!

---

## What I Learned

- 8051 microcontroller architecture and port configuration
- Embedded C programming with Keil uVision
- Matrix keypad scanning technique
- LCD 1602 interfacing and command sequences
- L293D motor driver for bidirectional motor control
- Active LOW current sinking for LEDs and buzzer
- Hardware simulation with Proteus
- Real hardware debugging and testing

---

## Future Enhancements

- Bluetooth/WiFi control via HC-05 or ESP8266
- Password change feature without re-flashing
- Multiple user passwords stored in EEPROM
- Fingerprint sensor as second authentication factor
- OTP based access via SMS

---

## Team

| Name |
|------|
| Aswathy R Nath |
| Alina Reju |
| Pavithra K |
| Alfin K Jaimon |
| Abhinand P Nandakumar |

*Electronics Department — Microcontroller Course Project*

---

## Project Status
- [x] Embedded C code complete
- [x] Proteus simulation verified
- [x] Hardware implementation tested
- [x] All 12 features working
- [ ] Bluetooth enhancement (planned)
- [ ] EEPROM password storage (planned)

