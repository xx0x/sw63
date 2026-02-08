#pragma once

#include "app/Settings.hpp"
#include "dev/DS3231.hpp"
#include <cstdint>
#include <array>

/**
 * Communication class for USB CDC serial communication
 * Handles protocol messages for setting/getting time and configuration
 */
class Communication
{
public:
    // Protocol command IDs
    enum class Command : uint8_t
    {
        SET_TIME = 0x01,
        GET_TIME = 0x02,
        SET_CONFIG = 0x03,
        GET_CONFIG = 0x04,
        GET_BATTERY_LEVEL = 0x05
    };

    // Response status codes
    enum class Status : uint8_t
    {
        OK = 0x00,
        ERROR = 0x01,
        INVALID_COMMAND = 0x02,
        INVALID_LENGTH = 0x03,
        INVALID_DATA = 0x04
    };

    // Protocol message structure
    // [Command ID][Length][Data...]
    static constexpr uint8_t MAX_MESSAGE_LENGTH = 32;
    static constexpr uint8_t HEADER_SIZE = 2; // Command + Length
    static constexpr size_t MAX_PACKET_SIZE = HEADER_SIZE + MAX_MESSAGE_LENGTH;

    /**
     * Initialize the communication interface
     * Sets up TinyUSB CDC and starts USB
     * @return true if successful
     */
    bool Init();

    /**
     * Process incoming data and handle protocol messages
     * Call this periodically in the main loop
     */
    void Process();

    /**
     * Check if USB is connected and configured
     * @return true if connected
     */
    bool IsConnected() const;

private:
    // Receive buffer for incoming messages
    std::array<uint8_t, MAX_PACKET_SIZE> rx_buffer_;
    size_t rx_index_ = 0;
    bool message_ready_ = false;

    /**
     * Handle a complete message
     * @param command Command ID
     * @param data Pointer to message data
     * @param length Length of data
     */
    void HandleMessage(Command command, const uint8_t* data, uint8_t length);

    /**
     * Send a response message
     * @param command Original command
     * @param status Response status
     * @param data Pointer to response data (optional)
     * @param length Length of response data
     */
    void SendResponse(Command command, Status status, const uint8_t* data = nullptr, uint8_t length = 0);

    /**
     * Handle SET_TIME command
     * @param data Pointer to DS3231::DateTime data
     * @param length Length of data
     */
    void HandleSetTime(const uint8_t* data, uint8_t length);

    /**
     * Handle GET_TIME command
     */
    void HandleGetTime();

    /**
     * Handle SET_CONFIG command
     * @param data Pointer to Settings::Config data
     * @param length Length of data
     */
    void HandleSetConfig(const uint8_t* data, uint8_t length);

    /**
     * Handle GET_CONFIG command
     */
    void HandleGetConfig();

    /**
     * Handle GET_BATTERY_LEVEL command
     */
    void HandleGetBatteryLevel();

    /**
     * Reset the receive buffer
     */
    void ResetReceiveBuffer();
};