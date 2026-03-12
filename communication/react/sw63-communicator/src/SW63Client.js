const COMMANDS = {
    // 0x01-0x0F = General
    GET_VERSION: 0x01,
    GET_BATTERY_LEVEL: 0x0B,
    // 0x10-0x1F = Time
    SET_TIME: 0x10,
    GET_TIME: 0x11,
    DISPLAY_TIME: 0x12,
    // 0x20-0x2F = Configuration
    SET_CONFIG: 0x20,
    GET_CONFIG: 0x21,
    GET_CONFIG_OPTIONS: 0x22
}

const STATUS_TEXT = {
    0x00: 'OK',
    0x01: 'ERROR',
    0x02: 'INVALID_COMMAND',
    0x03: 'INVALID_LENGTH',
    0x04: 'INVALID_DATA',
}

function pad2(value) {
    return String(value).padStart(2, '0')
}

async function readProtocolResponse(serialPort) {
    const reader = serialPort.readable.getReader()
    const buffer = []

    try {
        while (true) {
            const { value, done } = await reader.read()
            if (done) {
                throw new Error('Connection closed before full response was received.')
            }

            buffer.push(...value)

            if (buffer.length >= 3) {
                const dataLength = buffer[2]
                const messageLength = 3 + dataLength

                if (buffer.length >= messageLength) {
                    return buffer.slice(0, messageLength)
                }
            }
        }
    } finally {
        reader.releaseLock()
    }
}

export class SW63Client {
    constructor() {
        this.port = null
    }

    async connect() {
        if (!('serial' in navigator)) {
            throw new Error('Web Serial API is not available in this browser.')
        }

        const selectedPort = await navigator.serial.requestPort()

        if (!selectedPort.readable || !selectedPort.writable) {
            await selectedPort.open({ baudRate: 115200 })
        }

        this.port = selectedPort
    }

    async disconnect() {
        if (!this.port) {
            return
        }

        try {
            if (this.port.readable || this.port.writable) {
                await this.port.close()
            }
        } finally {
            this.port = null
        }
    }

    isConnected() {
        return Boolean(this.port)
    }

    async sendCommand(commandId, data = []) {
        if (!this.port) {
            throw new Error('Not connected to a device.')
        }

        if (!this.port.writable || !this.port.readable) {
            throw new Error('Serial port is not open.')
        }

        const writer = this.port.writable.getWriter()
        try {
            const request = new Uint8Array([commandId, data.length, ...data])
            await writer.write(request)
        } finally {
            writer.releaseLock()
        }

        const response = await readProtocolResponse(this.port)
        const [responseCommand, status, dataLength, ...responseData] = response

        if (responseCommand !== commandId) {
            throw new Error(`Unexpected response command: 0x${responseCommand.toString(16)}`)
        }

        if (status !== 0x00) {
            const label = STATUS_TEXT[status] ?? `UNKNOWN_STATUS_${status}`
            throw new Error(`Device returned error: ${label}`)
        }

        if (responseData.length !== dataLength) {
            throw new Error('Malformed response payload length.')
        }

        return responseData
    }

    async getConfig() {
        const data = await this.sendCommand(COMMANDS.GET_CONFIG)
        if (data.length !== 3) {
            throw new Error('GET_CONFIG returned invalid payload length.')
        }
        return {
            speed: data[0],
            language: data[1],
            style: data[2],
        }
    }

    async setConfig(speed, language, style) {
        await this.sendCommand(COMMANDS.SET_CONFIG, [speed, language, style])
    }

    async getTime() {
        const data = await this.sendCommand(COMMANDS.GET_TIME)
        return SW63Client.formatTime(data)
    }

    async setTime() {
        await this.sendCommand(COMMANDS.SET_TIME, SW63Client.getCurrentTimePayload())
        const formattedTime = await this.getTime()
        return formattedTime
    }

    async getBattery() {
        const data = await this.sendCommand(COMMANDS.GET_BATTERY_LEVEL)
        if (data.length !== 1) {
            throw new Error('GET_BATTERY_LEVEL returned invalid payload length.')
        }
        return data[0]
    }

    async displayTime() {
        await this.sendCommand(COMMANDS.DISPLAY_TIME)
    }

    async getConfigOptions(optionIndex) {
        const data = await this.sendCommand(COMMANDS.GET_CONFIG_OPTIONS, [optionIndex])
        // Decode response as UTF-8 string
        const decoder = new TextDecoder()
        const text = decoder.decode(new Uint8Array(data))
        // Split by semicolon and filter empty strings
        return text.split(';').filter((s) => s.length > 0)
    }

    async getVersion() {
        const data = await this.sendCommand(COMMANDS.GET_VERSION)
        // Decode response as UTF-8 string
        const decoder = new TextDecoder()
        const version = decoder.decode(new Uint8Array(data))
        return version
    }

    static formatTime(data) {
        if (!data || data.length !== 7) {
            return 'N/A'
        }

        const [hour, minute, second, day, month, yearLo, yearHi] = data
        const year = yearLo | (yearHi << 8)
        return `${year}-${pad2(month)}-${pad2(day)} ${pad2(hour)}:${pad2(minute)}:${pad2(second)}`
    }

    static getCurrentTimePayload() {
        const now = new Date()
        const year = now.getFullYear()
        return [
            now.getHours(),
            now.getMinutes(),
            now.getSeconds(),
            now.getDate(),
            now.getMonth() + 1,
            year & 0xff,
            (year >> 8) & 0xff,
        ]
    }
}
