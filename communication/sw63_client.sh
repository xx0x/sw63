#!/bin/bash

# SW63 Communication
# Automatically sets up virtual environment and runs the Python client

set -e  # Exit on any error

# Change to the script directory
cd "$(dirname "$0")"

# Function to the virtual environment and install dependencies
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
    
    # Install requirements directly
    echo "Installing Python dependencies..."
    pip install pyserial
    
    echo "✅ Setup complete!"
    echo ""
}

# Check if virtual environment exists
if [ ! -d "venv" ]; then
    setup_venv
fi

# Activate virtual environment and run the Python client
source venv/bin/activate
python sw63_client.py "$@"