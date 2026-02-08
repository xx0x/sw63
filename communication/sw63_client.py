#!/usr/bin/env python3
"""
SW63 Electronic Watch Communication Client

This script provides command-line interface to communicate with the SW63 watch
via USB CDC serial port. It can set/get time and configuration.

Usage examples:
    python sw63_client.py --set-time     # Set time from system clock
    python sw63_client.py --get-time     # Get time from watch
    python sw63_client.py --set-config SPEED LANGUAGE NUM_STYLE
    python sw63_client.py --get-config   # Get config from watch
    
    # Set config examples:
    python sw63_client.py --set-config 0 1 2
    python sw63_client.py --list-ports    # List available serial ports
"""

import serial
import serial.tools.list_ports
import struct
import argparse
import datetime
import sys
import time
from typing import Optional, Tuple

# Protocol constants
class Command:
    SET_TIME = 0x01
    GET_TIME = 0x02
    SET_CONFIG = 0x03
    GET_CONFIG = 0x04
    GET_BATTERY_LEVEL = 0x05
    DISPLAY_TIME = 0x06

class Status:
    OK = 0x00
    ERROR = 0x01
    INVALID_COMMAND = 0x02
    INVALID_LENGTH = 0x03
    INVALID_DATA = 0x04

# Time offset to compensate for transmission delay and display update time
# Transmission takes a few ms and the actual time display takes a few seconds,
# so it's good to set it a little bit into the future
TIME_OFFSET_SECONDS = 15

class SW63Client:
    """SW63 Watch Communication Client"""
    
    def __init__(self, port: str, timeout: float = 5.0):
        """
        Initialize client
        
        Args:
            port: Serial port name (e.g., '/dev/ttyACM0', 'COM3')
            timeout: Communication timeout in seconds
        """
        self.port = port
        self.timeout = timeout
        self.serial = None
        
    def connect(self) -> bool:
        """
        Connect to the watch
        
        Returns:
            True if connected successfully, False otherwise
        """
        try:
            self.serial = serial.Serial(
                port=self.port,
                baudrate=115200,  # CDC doesn't really use baudrate, but required
                timeout=self.timeout,
                write_timeout=self.timeout
            )
            time.sleep(0.1)  # Small delay for connection to establish
            print(f"Connected to {self.port}")
            return True
        except Exception as e:
            print(f"Error connecting to {self.port}: {e}")
            return False
    
    def disconnect(self):
        """Disconnect from the watch"""
        if self.serial and self.serial.is_open:
            self.serial.close()
            print("Disconnected")
    
    def send_command(self, command: int, data: bytes = b'') -> Tuple[bool, int, bytes]:
        """
        Send command to watch and receive response
        
        Args:
            command: Command ID
            data: Command data
            
        Returns:
            Tuple of (success, status, response_data)
        """
        if not self.serial or not self.serial.is_open:
            return False, Status.ERROR, b''
        
        # Create message: [Command][Length][Data...]
        message = struct.pack('BB', command, len(data)) + data
        
        try:
            # Send message
            self.serial.write(message)
            self.serial.flush()
            
            # Read response header: [Command][Status][Length]
            response_header = self.serial.read(3)
            if len(response_header) != 3:
                print("Error: Incomplete response header")
                return False, Status.ERROR, b''
            
            resp_command, status, resp_length = struct.unpack('BBB', response_header)
            
            # Verify command echo
            if resp_command != command:
                print(f"Error: Command mismatch. Sent: {command:02X}, Received: {resp_command:02X}")
                return False, Status.ERROR, b''
            
            # Read response data if any
            response_data = b''
            if resp_length > 0:
                response_data = self.serial.read(resp_length)
                if len(response_data) != resp_length:
                    print("Error: Incomplete response data")
                    return False, Status.ERROR, b''
            
            return True, status, response_data
            
        except Exception as e:
            print(f"Communication error: {e}")
            return False, Status.ERROR, b''
    
    def set_time(self, dt: Optional[datetime.datetime] = None) -> bool:
        """
        Set watch time
        
        Args:
            dt: Datetime to set. If None, uses current system time with an offset.
            
        Returns:
            True if successful
        """
        if dt is None:
            dt = datetime.datetime.now()
            # Add time offset to compensate for transmission and display delays
            dt = dt + datetime.timedelta(seconds=TIME_OFFSET_SECONDS)
        
        
        # Pack datetime according to DS3231::DateTime structure
        # struct DateTime { uint8_t hour, minute, second, day, month; uint16_t year; }
        data = struct.pack('<BBBBBH', 
                          dt.hour, dt.minute, dt.second, 
                          dt.day, dt.month, dt.year)
        
        success, status, _ = self.send_command(Command.SET_TIME, data)
        
        if success and status == Status.OK:
            print(f"Time set successfully: {dt.strftime('%Y-%m-%d %H:%M:%S')}")
            return True
        else:
            print(f"Failed to set time. Status: {status:02X}")
            return False
    
    def get_time(self) -> Optional[datetime.datetime]:
        """
        Get watch time
        
        Returns:
            Datetime object or None if failed
        """
        success, status, data = self.send_command(Command.GET_TIME)
        
        if success and status == Status.OK and len(data) == 7:
            # Unpack datetime from DS3231::DateTime structure
            hour, minute, second, day, month, year = struct.unpack('<BBBBBH', data)
            
            try:
                dt = datetime.datetime(year, month, day, hour, minute, second)
                print(f"Watch time: {dt.strftime('%Y-%m-%d %H:%M:%S')}")
                return dt
            except ValueError as e:
                print(f"Invalid datetime received: {e}")
                return None
        else:
            print(f"Failed to get time. Status: {status:02X}")
            return None
    
    def set_config(self, speed: int, language: int, num_style: int) -> bool:
        """
        Set watch configuration
        
        Args:
            speed: Speed setting (0-based index)
            language: Language setting (0-based index) 
            num_style: Number style setting (0-based index)
            
        Returns:
            True if successful
        """
        # Pack config according to Settings::Config structure
        # struct Config { uint8_t speed; Locale::Language language; Display::NumStyle num_style; }
        # Assuming enums are stored as uint8_t
        data = struct.pack('<BBB', speed, language, num_style)
        
        success, status, _ = self.send_command(Command.SET_CONFIG, data)
        
        if success and status == Status.OK:
            print(f"Config set successfully: speed={speed}, language={language}, num_style={num_style}")
            return True
        else:
            print(f"Failed to set config. Status: {status:02X}")
            return False
    
    def get_config(self) -> Optional[Tuple[int, int, int]]:
        """
        Get watch configuration
        
        Returns:
            Tuple of (speed, language, num_style) or None if failed
        """
        success, status, data = self.send_command(Command.GET_CONFIG)
        
        if success and status == Status.OK and len(data) == 3:
            speed, language, num_style = struct.unpack('<BBB', data)
            print(f"Watch config: speed={speed}, language={language}, num_style={num_style}")
            return speed, language, num_style
        else:
            print(f"Failed to get config. Status: {status:02X}")
            return None
    
    def get_battery_level(self) -> Optional[int]:
        """
        Get watch battery level
        
        Returns:
            Battery level as percentage (0-100) or None if failed
        """
        success, status, data = self.send_command(Command.GET_BATTERY_LEVEL)
        
        if success and status == Status.OK and len(data) == 1:
            battery_level = struct.unpack('<B', data)[0]
            print(f"Battery level: {battery_level}%")
            return battery_level
        else:
            print(f"Failed to get battery level. Status: {status:02X}")
            return None
    
    def display_time(self) -> bool:
        """
        Force the watch to display the time layer
        
        Returns:
            True if successful
        """
        success, status, _ = self.send_command(Command.DISPLAY_TIME)
        
        if success and status == Status.OK:
            print("Display time command sent successfully")
            return True
        else:
            print(f"Failed to send display time command. Status: {status:02X}")
            return False

def list_serial_ports():
    """List available serial ports"""
    ports = serial.tools.list_ports.comports()
    print("Available serial ports:")
    for port in ports:
        print(f"  {port.device} - {port.description}")
        if port.vid is not None:
            print(f"    VID: 0x{port.vid:04X}, PID: 0x{port.pid:04X}")

def find_sw63_port() -> Optional[str]:
    """Try to automatically find SW63 watch port"""
    ports = serial.tools.list_ports.comports()
    
    # Look for SW63 device (using the VID/PID from usb_descriptors.c)
    for port in ports:
        if port.vid == 0xCAFE and port.pid == 0x4001:
            return port.device
        # Also check for common CDC device patterns
        if "SW63" in port.description or "CDC" in port.description:
            return port.device
    
    return None

def main():
    parser = argparse.ArgumentParser(
        description="SW63 Electronic Watch Communication Client",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s --set-time                    # Set time from system clock
  %(prog)s --get-time                    # Get time from watch
  %(prog)s --set-config 0 1 2            # Set config (speed=0, lang=1, style=2)
  %(prog)s --get-config                  # Get current config
  %(prog)s --get-battery                 # Get battery level
  %(prog)s --display-time                # Force watch to show time
  %(prog)s --list-ports                  # List available ports
  %(prog)s --port COM3 --get-time        # Use specific port
        """
    )
    
    parser.add_argument('--port', help='Serial port (auto-detect if not specified)')
    parser.add_argument('--set-time', action='store_true', help='Set watch time from system clock')
    parser.add_argument('--get-time', action='store_true', help='Get watch time')
    parser.add_argument('--set-config', nargs=3, type=int, metavar=('SPEED', 'LANGUAGE', 'NUM_STYLE'),
                       help='Set watch configuration (all 3 values required)')
    parser.add_argument('--get-config', action='store_true', help='Get watch configuration')
    parser.add_argument('--get-battery', action='store_true', help='Get watch battery level')
    parser.add_argument('--display-time', action='store_true', help='Force watch to display time')
    parser.add_argument('--list-ports', action='store_true', help='List available serial ports')
    
    args = parser.parse_args()
    
    if args.list_ports:
        list_serial_ports()
        return
    
    # Find port
    port = args.port
    if not port:
        port = find_sw63_port()
        if not port:
            print("Error: Could not find SW63 watch. Please specify port manually with --port")
            list_serial_ports()
            return
    
    # Check if any action was specified
    if not (args.set_time or args.get_time or args.set_config or args.get_config or args.get_battery or args.display_time):
        parser.print_help()
        return
    
    # Connect to watch
    client = SW63Client(port)
    if not client.connect():
        return
    
    try:
        # Execute requested actions
        if args.set_time:
            client.set_time()
        
        if args.get_time:
            client.get_time()
        
        if args.set_config:
            speed, language, num_style = args.set_config
            client.set_config(speed, language, num_style)
        
        if args.get_config:
            client.get_config()
        
        if args.get_battery:
            client.get_battery_level()
        
        if args.display_time:
            client.display_time()
    
    finally:
        client.disconnect()

if __name__ == '__main__':
    main()