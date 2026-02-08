# SW63 Electronic Watch Communication Protocol

This document describes the communication protocol used between the SW63 Electronic Watch and external devices (computer, web applications) via USB CDC (Communication Device Class).

## Overview

The SW63 watch implements a simple binary protocol over USB CDC serial communication that allows:
- Setting and getting the current time
- Setting and getting watch configuration
- Compatible with standard serial terminals and web serial API

## Connection Details

- **Interface**: USB CDC (Virtual COM Port)
- **Baud Rate**: 115200 (Note: CDC doesn't strictly require baud rate, but this is the standard)
- **Data Bits**: 8
- **Parity**: None
- **Stop Bits**: 1
- **Flow Control**: None

## USB Device Descriptors

- **Vendor ID**: 0xCAFE (Placeholder - replace with official VID)
- **Product ID**: 0x4001 (Placeholder - replace with official PID)
- **Device Name**: "SW63 Electronic Watch"
- **Manufacturer**: "SW63 Team"

## Protocol Structure

### Message Format

All messages follow this structure:

```
[Command ID] [Length] [Data...]
```

- **Command ID**: 1 byte - Identifies the command type
- **Length**: 1 byte - Length of the data field (0-32)
- **Data**: 0-32 bytes - Command-specific data

### Response Format

All responses follow this structure:

```
[Command ID] [Status] [Length] [Data...]
```

- **Command ID**: 1 byte - Echo of the original command
- **Status**: 1 byte - Status code (see Status Codes section)
- **Length**: 1 byte - Length of the response data field (0-32)
- **Data**: 0-32 bytes - Response data (if any)

## Commands

### Command IDs

| Command | ID | Description |
|---------|-----|-------------|
| SET_TIME | 0x01 | Set the watch time |
| GET_TIME | 0x02 | Get the current watch time |
| SET_CONFIG | 0x03 | Set watch configuration |
| GET_CONFIG | 0x04 | Get watch configuration |
| GET_BATTERY_LEVEL | 0x05 | Get battery level (0-100%) |
| DISPLAY_TIME | 0x06 | Force watch to display time |
### Status Codes

| Status | Code | Description |
|--------|------|-------------|
| OK | 0x00 | Operation successful |
| ERROR | 0x01 | Generic error |
| INVALID_COMMAND | 0x02 | Unknown command ID |
| INVALID_LENGTH | 0x03 | Invalid data length |
| INVALID_DATA | 0x04 | Invalid data content |

## Command Details

### SET_TIME (0x01)

Sets the current date and time on the watch.

**Request:**
```
[0x01] [0x07] [hour] [minute] [second] [day] [month] [year_low] [year_high]
```

**Data Format:**
- `hour`: 0-23 (24-hour format)
- `minute`: 0-59
- `second`: 0-59
- `day`: 1-31
- `month`: 1-12
- `year`: 16-bit little-endian (e.g., 2024 = 0xE8 0x07)

**Response:**
```
[0x01] [status] [0x00]
```

**Example:**
Set time to 2024-08-18 14:30:25:
```
Request:  01 07 0E 1E 19 12 08 E8 07
Response: 01 00 00
```

### GET_TIME (0x02)

Gets the current date and time from the watch.

**Request:**
```
[0x02] [0x00]
```

**Response:**
```
[0x02] [status] [0x07] [hour] [minute] [second] [day] [month] [year_low] [year_high]
```

**Example:**
```
Request:  02 00
Response: 02 00 07 0E 1E 19 12 08 E8 07
```

### SET_CONFIG (0x03)

Sets the watch configuration.

**Request:**
```
[0x03] [0x03] [speed] [language] [num_style]
```

**Data Format:**
- `speed`: Speed setting index (0-based)
- `language`: Language setting index (0-based)
- `num_style`: Number style index (0-based)

**Response:**
```
[0x03] [status] [0x00]
```

**Example:**
Set speed=1, language=2, num_style=0:
```
Request:  03 03 01 02 00
Response: 03 00 00
```

### GET_CONFIG (0x04)

Gets the current watch configuration.

**Request:**
```
[0x04] [0x00]
```

**Response:**
```
[0x04] [status] [0x03] [speed] [language] [num_style]
```

**Example:**
```
Request:  04 00
Response: 04 00 03 01 02 00
```

### GET_BATTERY_LEVEL (0x05)

Gets the current battery level as a percentage.

**Request:**
```
[0x05] [0x00]
```

**Response:**
```
[0x05] [status] [0x01] [battery_level]
```

**Data Format:**
- `battery_level`: 0-100 (percentage)

**Example:**
```
Request:  05 00
Response: 05 00 01 4B  # Battery at 75%
```

### DISPLAY_TIME (0x06)

Forces the watch to display the time layer (normal time display). This command can be used to return to the time display from any other screen/layer.

**Request:**
```
[0x06] [0x00]
```

**Response:**
```
[0x06] [status] [0x00]
```

**Example:**
```
Request:  06 00
Response: 06 00 00  # Successfully switched to time display
```

## Error Handling

If an error occurs, the watch will respond with an appropriate status code:

- **INVALID_COMMAND (0x02)**: The command ID is not recognized
- **INVALID_LENGTH (0x03)**: The data length doesn't match expected length for the command
- **INVALID_DATA (0x04)**: The data contains invalid values (e.g., hour > 23)
- **ERROR (0x01)**: Generic error (e.g., RTC communication failure)

## Data Structures

### DateTime Structure (C++)

```cpp
struct DateTime {
    uint8_t hour;   // 0-23 (24-hour format)
    uint8_t minute; // 0-59
    uint8_t second; // 0-59
    uint8_t day;    // 1-31
    uint8_t month;  // 1-12
    uint16_t year;  // Full year (e.g., 2024)
};
```

### Config Structure (C++)

```cpp
struct Config {
    uint8_t speed;             // Speed setting index
    Locale::Language language; // Language enum (stored as uint8_t)
    Display::NumStyle num_style; // Number style enum (stored as uint8_t)
};
```

## Usage Examples

### Python Client

```bash
# Set time from system clock
python sw63_client.py --set-time

# Get current time
python sw63_client.py --get-time
// Get battery level
./sw63_client.sh --get-battery
# Get battery level
python sw63_client.py --get-battery

# Set configuration (speed=0, language=1, num_style=2)
python sw63_client.py --set-config 0 1 2

# Get current configuration
python sw63_client.py --get-config

# List available serial ports
python sw63_client.py --list-ports
```

### Manual Serial Communication

Using a serial terminal (e.g., `screen`, `minicom`, or PuTTY):

1. Connect to the device (Linux: `/dev/ttyACM0`, Windows: `COM3`, etc.)
2. Send hex bytes according to the protocol

## Web Serial Compatibility

This protocol is designed to be compatible with the [Web Serial API](https://developer.mozilla.org/en-US/docs/Web/API/Serial), allowing web applications to communicate directly with the watch:

```javascript
// Request access to serial port
const port = await navigator.serial.requestPort();

// Connect with appropriate settings
await port.open({ baudRate: 115200 });

// Send GET_TIME command
const writer = port.writable.getWriter();
const getTimeCommand = new Uint8Array([0x02, 0x00]);
await writer.write(getTimeCommand);

// Read response
const reader = port.readable.getReader();
const { value } = await reader.read();
// Parse response according to protocol...
```

## Implementation Notes

1. **Endianness**: Multi-byte values (like year) are stored in little-endian format
2. **Timeout**: Commands should complete within 5 seconds
3. **Buffer Size**: Maximum message length is 255 bytes for both commands and responses
4. **Concurrency**: Only one command should be active at a time
5. **Connection**: The watch can be detected by VID/PID or device description containing "SW63"

## Future Extensions

The protocol is designed to be extensible. Future commands could include:
- Firmware version query
- Display brightness control
- Alarm settings
- Configuration backup/restore

New commands should use IDs starting from 0x07 and follow the same message structure.