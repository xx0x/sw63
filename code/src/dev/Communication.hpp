/*
MIT License

Copyright (c) 2026 Vaclav Mach (xx0x)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "app/Settings.hpp"
#include "dev/DS3231.hpp"
#include <array>
#include <cstdint>

/**
 * @brief Handles USB CDC protocol commands and responses.
 * @author Vaclav Mach (xx0x)
 * @date 2026-02-08
 */
class Communication
{
public:
    /**
     * @brief Lists protocol command identifiers accepted by the device.
     */
    enum class Command : uint8_t
    {
        // 0x01-0x0F = General
        GET_VERSION = 0x01,
        DISPLAY_INTRO = 0x02,
        GET_BATTERY_LEVEL = 0x0B,
        // 0x10-0x1F = Time
        SET_TIME = 0x10,
        GET_TIME = 0x11,
        DISPLAY_TIME = 0x12,
        // 0x20-0x2F = Configuration
        SET_CONFIG = 0x20,
        GET_CONFIG = 0x21,
        GET_CONFIG_OPTION_VALUES = 0x22,
        SET_CONFIG_OPTION = 0x23,
        GET_CONFIG_OPTION = 0x24,
    };

    /**
     * @brief Lists protocol response status codes.
     */
    enum class Status : uint8_t
    {
        OK = 0x00,
        ERROR = 0x01,
        INVALID_COMMAND = 0x02,
        INVALID_LENGTH = 0x03,
        INVALID_DATA = 0x04
    };

    /**
     * @brief Maximum payload size for a protocol message.
     */
    static constexpr uint8_t MAX_MESSAGE_LENGTH = 250;

    /**
     * @brief Size of protocol header in bytes.
     */
    static constexpr uint8_t HEADER_SIZE = 2; // Command + Length

    /**
     * @brief Maximum serialized packet size including header.
     */
    static constexpr size_t MAX_PACKET_SIZE = HEADER_SIZE + MAX_MESSAGE_LENGTH;

    /**
     * @brief Initializes USB communication interface, sets up TinyUSB CDC, and prepares for message processing.
     * @return True when initialization succeeds.
     */
    bool Init();

    /**
     * @brief Processes incoming protocol packets. Should be called regularly from the main loop to handle USB events and dispatch messages.
     */
    void Process();

    /**
     * @brief Checks whether USB CDC connection is active.
     * @return True when USB is connected and configured.
     */
    bool IsConnected() const;

private:
    // Receive buffer for incoming messages
    std::array<uint8_t, MAX_PACKET_SIZE> rx_buffer_;
    size_t rx_index_ = 0;
    bool message_ready_ = false;

    /**
     * Handles a complete message.
     * @param command Command ID
     * @param data Pointer to message data
     * @param length Length of data
     */
    void HandleMessage(Command command, const uint8_t *data, uint8_t length);

    /**
     * Sends a response message.
     * @param command Original command
     * @param status Response status
     * @param data Pointer to response data (optional)
     * @param length Length of response data
     */
    void SendResponse(Command command, Status status, const uint8_t *data = nullptr, uint8_t length = 0);

    /**
     * Handles SET_TIME command.
     * @param data Pointer to DS3231::DateTime data
     * @param length Length of data
     */
    void HandleSetTime(const uint8_t *data, uint8_t length);

    /**
     * Handles GET_TIME command.
     */
    void HandleGetTime();

    /**
     * Handles SET_CONFIG command.
     * @param data Pointer to Settings::Config data
     * @param length Length of data
     */
    void HandleSetConfig(const uint8_t *data, uint8_t length);

    /**
     * Handles GET_CONFIG command.
     */
    void HandleGetConfig();

    /**
     * @brief Handles SET_CONFIG_OPTION command.
     * @param option Configuration option index.
     * @param value New option value.
     */
    void HandleSetConfigOption(uint8_t option, uint8_t value);

    /**
     * @brief Handles GET_CONFIG_OPTION command.
     * @param option Configuration option index.
     */
    void HandleGetConfigOption(uint8_t option);

    /**
     * @brief Handles GET_BATTERY_LEVEL command.
     */
    void HandleGetBatteryLevel();

    /**
     * @brief Handles DISPLAY_TIME command.
     */
    void HandleDisplayTime();

    /**
     * @brief Handles DISPLAY_INTRO command.
     */
    void HandleDisplayIntro();

    /**
     * @brief Handles GET_CONFIG_OPTION_VALUES command.
     * @param option Configuration option index.
     */
    void HandleGetConfigOptionValues(uint8_t option);

    /**
     * @brief Handles GET_VERSION command.
     */
    void HandleGetVersion();

    /**
     * @brief Resets receive buffer state.
     */
    void ResetReceiveBuffer();
};