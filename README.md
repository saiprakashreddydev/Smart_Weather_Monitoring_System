# SWMS V3.0 — Smart Weather Monitoring System

A real-time IoT device that reads temperature, displays time & date on an LCD screen, and automatically uploads sensor data to the internet — all running on a tiny STM32 microcontroller.

---

## What Does This Project Do?

Imagine a small electronic box that:

1. **Shows the current time and date** on a small screen, updated every second.
2. **Reads room temperature** using a sensor every 5 seconds and shows it on the screen.
3. **Sends that temperature data to a web server** over Wi-Fi automatically, so you can monitor it remotely.
4. **Lets you set the clock** using three physical buttons (Up, Down, Enter) — like setting the time on a microwave.

---

## Hardware Components

| Component | What It Is | What It Does in This Project |
|---|---|---|
| **STM32F401RBT6** | The brain (microcontroller) | Runs all the logic |
| **LM35** | Temperature sensor | Measures room temperature in °C |
| **DS1307 / DS3231** | Real-Time Clock (RTC) module | Keeps track of time and date even when powered off |
| **16x2 LCD** | Small character display | Shows time, date, and temperature |
| **ESP8266** | Wi-Fi module | Connects to your Wi-Fi and sends data to internet |
| **3 Push Buttons** | UP, DOWN, ENTER | Let you set the time/date manually |

---

## How the Screen Looks

```
 HH:MM:SS    XX°C
 DD/MM/YY
```

- Top-left: current time (Hours:Minutes:Seconds)
- Top-right: temperature in Celsius
- Bottom-left: current date (Day/Month/Year)

---

## How It Works — Step by Step

### On Power-Up
1. The STM32 chip starts up and sets up all peripherals (LCD, sensors, Wi-Fi module, clock).
2. The LCD is initialized and cleared.
3. The ESP8266 Wi-Fi module is initialized with AT commands and connects to the Wi-Fi network.
4. Four background tasks are started and run simultaneously.

### The Four Running Tasks

```
┌─────────────────────────────────────────────────────────────────┐
│  Task 1 — Every 1 Second                                        │
│  Read time & date from RTC chip → Display on LCD                │
├─────────────────────────────────────────────────────────────────┤
│  Task 2 — Every 5 Seconds                                       │
│  Read temperature from LM35 → Display on LCD → Upload to server │
├─────────────────────────────────────────────────────────────────┤
│  Task 3 — Background                                            │
│  Internet status check (reserved for future use)                │
├─────────────────────────────────────────────────────────────────┤
│  Task 4 — On Button Press                                       │
│  Enter configuration mode → Let user set time/date via buttons  │
└─────────────────────────────────────────────────────────────────┘
```

### Setting the Time (Configuration Mode)
Press the **ENTER** button to enter setup mode. The LCD will guide you through entering:
1. Seconds → Minutes → Hours
2. Day of week → Day → Month → Year

Use **UP** and **DOWN** buttons to change values. Press **ENTER** to confirm each field. When all 7 fields are set, the new time/date is written to the RTC chip and saved permanently.

### Sending Data to the Internet
Every 5 seconds, the ESP8266 Wi-Fi module opens a TCP connection to the server and sends an HTTP GET request like:

```
GET /page.php?temp=27&hum=30&dev=36
```

This uploads the current temperature to the web server so it can be logged or displayed on a webpage.

---

## Project Structure

```
SWMS_V3.0/
├── Core/
│   ├── Src/
│   │   ├── main.c          ← Main program: tasks, Wi-Fi init, RTC config logic
│   │   ├── lcd.c           ← LCD driver: writes characters to the screen
│   │   ├── freertos.c      ← FreeRTOS memory setup
│   │   └── stm32f4xx_it.c  ← Timer interrupts (1-second and 5-second flags)
│   └── Inc/
│       ├── main.h          ← Pin definitions (LCD, buttons, etc.)
│       └── lcd.h           ← LCD function declarations
├── Drivers/                ← STM32 HAL (Hardware Abstraction Layer) library
├── Middlewares/            ← FreeRTOS source files
└── SWMS_V3.0.ioc           ← STM32CubeIDE pin configuration file
```

---

## Key Technical Details

| Item | Value |
|---|---|
| Microcontroller | STM32F401RBT6 (ARM Cortex-M4) |
| Clock Speed | 16 MHz |
| RTOS | FreeRTOS (via CMSIS-RTOS API) |
| LCD Interface | 4-bit parallel |
| RTC Interface | I2C (address 0x68) |
| Wi-Fi Module | ESP8266 via UART at 115200 baud |
| Temperature Conversion | `(ADC_value × 3000) / 40960` → °C |
| IDE | STM32CubeIDE |
| Wi-Fi Credentials | SSID: `SAI`, Password: `12345678` |

---

## How Timing Works

A hardware timer (TIM10) fires an interrupt every 1 millisecond. A software counter inside that interrupt tracks:
- Every **1000 ticks** (1 second) → triggers Task 1 to refresh the LCD clock
- Every **5000 ticks** (5 seconds) → triggers Task 2 to read temperature and upload data

This way the microcontroller is never sitting idle in a blocking loop — FreeRTOS handles all the scheduling.

---

## Getting Started

### Requirements
- **STM32CubeIDE** (free, from STMicroelectronics)
- An **ST-Link** programmer/debugger to flash the board
- The hardware components listed above wired up correctly

### Build & Flash
1. Open STM32CubeIDE.
2. Import this project: `File → Open Projects from File System`.
3. Build: `Project → Build All` (or press `Ctrl+B`).
4. Flash to board: `Run → Debug` (or press `F11`).

### Pin Connections (Quick Reference)

| Signal | MCU Pin | Connect To |
|---|---|---|
| LCD D0–D3 | PB0, PB1, PB2, PB3 | LCD data pins |
| LCD RS | PB4 | LCD RS pin |
| LCD RW | PB5 | LCD RW pin |
| LCD EN | PB8 | LCD Enable pin |
| RTC SCL | PB6 | DS1307 SCL |
| RTC SDA | PB7 | DS1307 SDA |
| Temperature | PC0 | LM35 output |
| Wi-Fi TX | PC6 | ESP8266 RX |
| Wi-Fi RX | PC7 | ESP8266 TX |
| Button UP | PC8 | Push button |
| Button DOWN | PC9 | Push button |
| Button ENTER | PC10 | Push button |

---

## Version History

| Version | Notes |
|---|---|
| V3.0 | FreeRTOS multitasking, Wi-Fi data upload, RTC config via buttons |
