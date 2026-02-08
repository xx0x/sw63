# SW63 Watch Communication Tools

This folder contains tools and documentation for communicating with the SW63 Electronic Watch via USB CDC.

## Files

- **`sw63_client.sh`** - All-in-one script that sets up environment and runs the client
- **`sw63_client.py`** - Python communication client
- **`PROTOCOL.md`** - Complete protocol specification and documentation

## Quick Start

The SW63 client automatically sets up everything on first run - no manual setup required!

### Basic Usage

```bash
# Set time from computer clock
./sw63_client.sh --set-time

# Get current watch time  
./sw63_client.sh --get-time

# Set configuration (speed=0, language=1, style=2)
./sw63_client.sh --set-config 0 1 2

# Get current configuration
./sw63_client.sh --get-config

# Get battery level
./sw63_client.sh --get-battery
```

On first run, the script will automatically:
- Create a Python virtual environment
- Install required dependencies (pyserial)
- Run your command

### Prerequisites

- Python 3.6 or later (the script will check for this)

### Find Your Device

If the auto-detection doesn't work:

```bash
# List available ports
./sw63_client.sh --list-ports

# Use specific port
./sw63_client.sh --port /dev/ttyACM0 --get-time  # Linux/macOS
./sw63_client.sh --port COM3 --get-time          # Windows
```

## Protocol Details

See [PROTOCOL.md](PROTOCOL.md) for complete protocol specification, including:
- Message format
- Command reference
- Error codes
- Web Serial compatibility
- Implementation examples

## Web Development

The protocol is designed to work with the Web Serial API for browser-based applications. See the protocol documentation for JavaScript examples.

## Troubleshooting

**Device not found:**
- Ensure the watch is connected and recognized by your OS
- Check that USB CDC drivers are installed
- Try a different USB cable or port

**Permission denied (Linux):**
- Add your user to the dialout group: `sudo usermod -a -G dialout $USER`
- Log out and back in after adding to group

**Communication errors:**
- Ensure only one application is accessing the serial port
- Check that the watch firmware includes USB CDC support
- Try disconnecting and reconnecting the watch