#!/bin/bash

# SW63 Client Script
# Automatically sets up virtual environment and runs the Python client

set -e  # Exit on any error

# Change to the script directory
cd "$(dirname "$0")"

# Function to create requirements.txt content
setup_venv() {
    echo "Setting up SW63 communication environment..."
    
    # Check if Python 3 is available
    if ! command -v python3 &> /dev/null; then
        echo "Error: python3 is required but not installed."
        echo "Please install Python 3.6 or later."
        exit 1
    fi
    
    # Create virtual environment
    echo "Creating Python virtual environment..."
    python3 -m venv venv
    
    # Activate virtual environment
    echo "Activating virtual environment..."
    source venv/bin/activate
    
    # Upgrade pip
    echo "Upgrading pip..."
    pip install --upgrade pip
    
    # Install requirements directly (embedded in script)
    echo "Installing Python dependencies..."
    pip install pyserial>=3.5
    
    echo "✅ Setup complete!"
    echo ""
}

# Check if virtual environment exists
if [ ! -d "venv" ]; then
    setup_venv
fi

# Check if virtual environment has the required packages
if [ ! -f "venv/bin/python" ] || ! venv/bin/python -c "import serial" &>/dev/null; then
    echo "Virtual environment exists but dependencies are missing."
    echo "Reinstalling dependencies..."
    source venv/bin/activate
    pip install --upgrade pip
    pip install pyserial>=3.5
    echo "✅ Dependencies updated!"
fi

# Activate virtual environment and run the Python client
source venv/bin/activate
python sw63_client.py "$@"