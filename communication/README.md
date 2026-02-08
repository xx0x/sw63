# SW63 Communication

Tools for communicating with the SW63 via USB CDC.

## Usage

```bash
# Set time from computer clock
./sw63_client.sh --set-time

# Get current watch time  
./sw63_client.sh --get-time

# Set configuration (speed=0, language=1, style=2)
# Note that the values are zero-indexed (the watch shows them as one-indexed)
./sw63_client.sh --set-config 0 1 2

# Get current configuration
./sw63_client.sh --get-config

# Get battery level
./sw63_client.sh --get-battery

# List available ports
./sw63_client.sh --list-ports
```

The script automatically sets up Python environment and dependencies on first run.

## Protocol Specification

### Message Format

**Request:**
```
[Command ID] [Data Length] [Data...]
```

**Response:**
```
[Command ID] [Status] [Data Length] [Data...]
```

### Commands

| Command           | ID   | Data Length | Description                |
| ----------------- | ---- | ----------- | -------------------------- |
| SET_TIME          | 0x01 | 0x07        | Set the time               |
| GET_TIME          | 0x02 | 0x00        | Get the current time       |
| SET_CONFIG        | 0x03 | 0x03        | Set configuration          |
| GET_CONFIG        | 0x04 | 0x00        | Get configuration          |
| GET_BATTERY_LEVEL | 0x05 | 0x00        | Get battery level (0-100%) |
| DISPLAY_TIME      | 0x06 | 0x00        | Force display time         |

### Status Codes

| Status          | Code | Description          |
| --------------- | ---- | -------------------- |
| OK              | 0x00 | Operation successful |
| ERROR           | 0x01 | Generic error        |
| INVALID_COMMAND | 0x02 | Unknown command ID   |
| INVALID_LENGTH  | 0x03 | Invalid data length  |
| INVALID_DATA    | 0x04 | Invalid data content |

### Date format (SET_TIME, GET_TIME)

| Byte | Field  | Range | Description                                   |
| ---- | ------ | ----- | --------------------------------------------- |
| 0    | hour   | 0-23  | 24-hour format                                |
| 1    | minute | 0-59  | Minutes                                       |
| 2    | second | 0-59  | Seconds                                       |
| 3    | day    | 1-31  | Day of month                                  |
| 4    | month  | 1-12  | Month (1=January)                             |
| 5-6  | year   | -     | 16-bit little-endian (e.g., 2024 = 0xE8 0x07) |

### Examples

**Set time to 2024-08-18 14:30:25:**
```
Request:  01 07 0E 1E 19 12 08 E8 07
Response: 01 00 00
```

**Get current time:**
```
Request:  02 00
Response: 02 00 07 0E 1E 19 12 08 E8 07
```

**Set config (speed=1, language=2, style=0):**
```
Request:  03 03 01 02 00
Response: 03 00 00
```

**Get battery level:**
```
Request:  05 00
Response: 05 00 01 4B  # 75%
```

## Troubleshooting

**Device not found:**
- Ensure watch is connected and USB CDC drivers installed
- Try different USB cable/port

**Permission denied (Linux):**
- Add user to dialout group: `sudo usermod -a -G dialout $USER`
- Log out and back in

**Communication errors:**
- Ensure only one app accesses the serial port
- Try disconnecting/reconnecting the watch