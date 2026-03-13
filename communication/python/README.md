# SW63 Communication: Python Client

Tools for communicating with the SW63 via USB CDC.

## Usage

```bash
# Set time from computer clock
./sw63_client.sh --set-time

# Get current watch time  
./sw63_client.sh --get-time

# Set configuration (speed=0, language=1, style=2)
# Note that the values are zero-indexed (the watch and react app shows them as one-indexed)
./sw63_client.sh --set-config 0 1 2

# Get current configuration
./sw63_client.sh --get-config

# Set one config field (option=1(language), value=2)
./sw63_client.sh --set-config-option 1 2

# Get one config field value (option=1(language))
./sw63_client.sh --get-config-option 1

# Get possible values for one config field as semicolon separated strings:
# 0=speed, 1=language, 2=num_style
./sw63_client.sh --get-config-options 1

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

| Command                  | ID   | Data Length | Description                                          |
| ------------------------ | ---- | ----------- | ---------------------------------------------------- |
| GET_VERSION              | 0x01 | 0x00        | Get firmware version                                 |
| DISPLAY_INTRO            | 0x02 | 0x00        | Displays the party animation                         |
| GET_BATTERY_LEVEL        | 0x0B | 0x00        | Get battery level (0-100%)                           |
| SET_TIME                 | 0x10 | 0x07        | Set the time                                         |
| GET_TIME                 | 0x11 | 0x00        | Get the current time                                 |
| DISPLAY_TIME             | 0x12 | 0x00        | Force display time                                   |
| SET_CONFIG               | 0x20 | 0x03        | Set configuration                                    |
| GET_CONFIG               | 0x21 | 0x00        | Get configuration                                    |
| GET_CONFIG_OPTION_VALUES | 0x22 | 0x01        | Get semicolon-separated option values for one config field |
| SET_CONFIG_OPTION        | 0x23 | 0x02        | Set one config field (option, value)                 |
| GET_CONFIG_OPTION        | 0x24 | 0x01        | Get one config field value (option)                  |

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
Request:  10 07 0E 1E 19 12 08 E8 07
Response: 10 00 00
```

**Get current time:**
```
Request:  11 00
Response: 11 00 07 0E 1E 19 12 08 E8 07
```

**Set config (speed=1, language=2, style=0):**
```
Request:  20 03 01 02 00
Response: 20 00 00
```

**Set one config option (option=1/language, value=2):**
```
Request:  23 02 01 02
Response: 23 00 00
```

**Get one config option (option=1/language):**
```
Request:  24 01 01
Response: 24 00 01 02
```

**Get language options (option=1):**
```
Request:  22 01 01
Response: 22 00 NN 43 7A 65 63 68 2C ...
```

The response payload is UTF-8 text with values separated by semicolons.
Option index mapping:
- `0`: speed options
- `1`: language options
- `2`: number style options

**Get battery level:**
```
Request:  0B 00
Response: 0B 00 01 4B  # 75%
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