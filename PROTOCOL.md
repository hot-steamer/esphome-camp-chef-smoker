# Technical Protocol Specification: Smoker Controller Serial Protocol

---

## 1. Protocol Overview

This document defines the serial communication protocol for interfacing with the smoker controller hardware. This is a **monitoring-only** serial protocol designed strictly for data telemetry and state observation.
This is likely only accurate for controllers with firmware revision F1NW V1.0.8.  Smokers that come with WiFi installed will very likely have a very different protocol.

> ⚠️ **Timing Requirement**
>
> Failure to adhere to the **200 ms (0.2 seconds) inter-packet delay** may result in:
> - Communication timeouts  
> - General system instability  


---

## 2. Physical Frame Structure

The protocol utilizes a binary framing format to identify packet boundaries within the serial bitstream. The controller relies on specific start and end delimiters.

| Component   | Size   | Description                          |
|------------|--------|--------------------------------------|
| Start Byte | 1 Byte | Constant value `0xFE`               |
| Opcode     | 1 Byte | Command or data identifier          |
| Payload    | Variable | Data associated with the opcode   |
| End Byte   | 1 Byte | Constant value `0xFF`               |

---

## 3. Data Encoding and Numerical Representation

Multi-byte numerical values (e.g., timers, temperatures) are encoded as **digit sequences**, not binary integers.

### Conversion Algorithm

To reconstruct the integer:
value = (value * 10) + byte_value

### Example

To transmit the integer `345`:
[0x03, 0x04, 0x05]



Step-by-step:
1. Initialize `value = 0`
2. `(0 * 10) + 3 = 3`
3. `(3 * 10) + 4 = 34`
4. `(34 * 10) + 5 = 345`

### Padding Rule

Fixed-length fields use leading zeros:

| Value | Encoded (3 bytes)       |
|-------|-------------------------|
| 45    | `[0x00, 0x04, 0x05]`    |

---

## 4. Opcode Reference Library

---

### 4.1 System Information and Configuration

| Opcode | Name                              | Description |
|--------|-----------------------------------|-------------|
| `0x07` | Config Table Index / System State | Returns configuration index (typically `0x01`) |
| `0x08` | Capabilities / Limits             | Hardware limits and factory defaults |
| `0x34` | Firmware Version                  | ASCII-encoded firmware version string |

---

### 4.2 Status and Telemetry (`0x11`)

Provides a comprehensive status block.

| Offset   | Parameter             | Description |
|----------|----------------------|-------------|
| 0–2      | Pit Temperature      | Current smoker temperature |
| 3–5      | Setpoint Temperature | Target temperature |
| 12–14    | Probe 1 Temperature  | Food probe 1 |
| 15–17    | Probe 2 Temperature  | Food probe 2 |
| 25       | Smoke Level          | User smoke setting |
| 26       | System State         | `0: STARTUP`, `1: IDLE`, `2: RUNNING`, `3: FEED`, `4: SHUTDOWN` |
| 28–30    | Error Flags          | Diagnostic bits |
| 34–35    | Probe Validity       | Probe connectivity |
| 38–40    | Component States     | Auger, Fan, Igniter status |

---

### 4.3 Timer and Counter Opcodes

| Opcode | Name              | Description |
|--------|-------------------|-------------|
| `0x50` | Startup Timer     | Remaining seconds in 6-minute startup |
| `0x51` | Feed Timer        | Remaining seconds in 7-minute feed cycle |
| `0x52` | Shutdown Timer    | Remaining seconds in 20-minute cooldown |
| `0x53` | Cook Time Counter | Total elapsed cook time |

#### Cook Time Counter Format (`0x53`)

9-byte payload split into 3 segments:

| Bytes | Meaning |
|-------|--------|
| 0–2   | Hours (HHH) |
| 3–5   | Minutes (MMM) |
| 6–8   | Seconds (SSS) |

Each segment uses digit encoding.

---

### 4.4 Utility and Miscellaneous Opcodes

| Opcode | Name                     | Description |
|--------|--------------------------|-------------|
| `0x25` | Echo                    | Returns payload verbatim |
| `0x22` | Display Buffer Write    | Writes to display buffer (no immediate refresh, unsure how to cause it to be written to screen, might only be used during OTA updates) |
| `0x54` | Multi-byte Command      | Requires sub-opcodes |
| `0x23` | Reserved / Unknown      | Function not determined |
| `0x24` | Reserved / Unknown      | Function not determined |

#### `0x54` Sub-opcodes (not figured out yet)

| Format                | Description |
|----------------------|-------------|
| `[0x54, 0x01, 0xXX]` | Requires additional data byte |
| `[0x54, 0x02]`       | Standalone |
| `[0x54, 0x03]`       | Standalone |

---

## 5. Critical Safety Warnings: Dangerous Opcodes

> 🚨 **WARNING: UNSTABLE AND DESTRUCTIVE COMMANDS**

The following opcodes interact with low-level memory or firmware systems. Improper use may permanently damage the controller.

| Opcode(s)              | Risk |
|------------------------|------|
| `0x12`                 | **CRITICAL** — confirmed to brick hardware |
| `0x40`                 | OTA update — may corrupt firmware, has sub-op-codes 0x01 and 0x02 |
| `0x29–0x32`            | Direct memory writes of 0x80 bytes — likely used for OTA updates |

> ❗ These opcodes must **never** be used in monitoring implementations.

---

## 6. Error Handling and Resilience

The controller uses a **passive error-handling model**:

- No NACK responses
- No explicit error packets

### Behavior

| Condition                          | Result |
|----------------------------------|--------|
| Invalid framing (`0xFE ... 0xFF`) | Ignored |
| Unknown opcode                   | Ignored |

### Client Responsibilities

Clients must implement:

- Timeout handling
- Retry logic
- Adherence to **200 ms inter-packet delay**

---

## Summary

This protocol is:

- Binary framed (`0xFE ... 0xFF`)
- Digit-encoded (not binary integers)
- Passive in error handling
- **Unsafe for control operations**

It is intended strictly for **telemetry and monitoring**.

---