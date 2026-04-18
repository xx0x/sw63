#!/bin/sh

if [ ! -d "code/build" ]; then
    echo "Error: code/build directory does not exist." >&2
    exit 1
fi

# Flash the firmware to the device (needs to be built first)
while true; do
    make -C code/build flash-with-bootloader || exit $?

    while true; do
        printf "Do you want to flash again? (y/n) "
        read -r answer

        case "${answer:-y}" in
            y|Y)
                break
                ;;
            n|N)
                exit 0
                ;;
            *)
                echo "Please answer y or n."
                ;;
        esac
    done
done
