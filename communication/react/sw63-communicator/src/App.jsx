
import { useMemo, useState } from 'react'

const COMMANDS = {
    SET_TIME: 0x01,
    GET_TIME: 0x02,
    SET_CONFIG: 0x03,
    GET_CONFIG: 0x04,
    GET_BATTERY_LEVEL: 0x05,
    DISPLAY_TIME: 0x06,
}

const STATUS_TEXT = {
    0x00: 'OK',
    0x01: 'ERROR',
    0x02: 'INVALID_COMMAND',
    0x03: 'INVALID_LENGTH',
    0x04: 'INVALID_DATA',
}

function getCurrentTimePayload() {
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

function pad2(value) {
    return String(value).padStart(2, '0')
}

function formatWatchTime(data) {
    if (!data || data.length !== 7) {
        return 'N/A'
    }

    const [hour, minute, second, day, month, yearLo, yearHi] = data
    const year = yearLo | (yearHi << 8)
    return `${year}-${pad2(month)}-${pad2(day)} ${pad2(hour)}:${pad2(minute)}:${pad2(second)}`
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

function App() {
    const [port, setPort] = useState(null)
    const [isBusy, setIsBusy] = useState(false)
    const [errorMessage, setErrorMessage] = useState('')
    const [statusMessage, setStatusMessage] = useState('Disconnected')

    const [speed, setSpeed] = useState(0)
    const [language, setLanguage] = useState(0)
    const [style, setStyle] = useState(0)
    const [watchTime, setWatchTime] = useState('N/A')
    const [batteryLevel, setBatteryLevel] = useState('N/A')

    const configOptions = useMemo(() => [0, 1, 2, 3, 4], [])

    const isConnected = Boolean(port)

    async function sendCommand(activePort, commandId, data = []) {
        if (!activePort) {
            throw new Error('Not connected to a device.')
        }

        if (!activePort.writable || !activePort.readable) {
            throw new Error('Serial port is not open.')
        }

        const writer = activePort.writable.getWriter()
        try {
            const request = new Uint8Array([commandId, data.length, ...data])
            await writer.write(request)
        } finally {
            writer.releaseLock()
        }

        const response = await readProtocolResponse(activePort)
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

    async function loadAllData(activePort) {
        const configData = await sendCommand(activePort, COMMANDS.GET_CONFIG)
        if (configData.length !== 3) {
            throw new Error('GET_CONFIG returned invalid payload length.')
        }
        setSpeed(configData[0])
        setLanguage(configData[1])
        setStyle(configData[2])

        const timeData = await sendCommand(activePort, COMMANDS.GET_TIME)
        setWatchTime(formatWatchTime(timeData))

        const batteryData = await sendCommand(activePort, COMMANDS.GET_BATTERY_LEVEL)
        if (batteryData.length !== 1) {
            throw new Error('GET_BATTERY_LEVEL returned invalid payload length.')
        }
        setBatteryLevel(String(batteryData[0]))
    }

    async function connect() {
        if (!('serial' in navigator)) {
            setErrorMessage('Web Serial API is not available in this browser.')
            return
        }

        let selectedPort = null

        setIsBusy(true)
        setErrorMessage('')
        setStatusMessage('Connecting...')

        try {
            selectedPort = await navigator.serial.requestPort()

            if (!selectedPort.readable || !selectedPort.writable) {
                await selectedPort.open({ baudRate: 115200 })
            }

            setStatusMessage('Connected. Loading watch data...')

            await loadAllData(selectedPort)
            setPort(selectedPort)
            setStatusMessage('Connected')
        } catch (error) {
            if (selectedPort && (selectedPort.readable || selectedPort.writable)) {
                try {
                    await selectedPort.close()
                } catch {
                    // Ignore close errors and preserve original connection error.
                }
            }

            setStatusMessage('Disconnected')
            setPort(null)
            setErrorMessage(error instanceof Error ? error.message : String(error))
        } finally {
            setIsBusy(false)
        }
    }

    async function disconnect() {
        if (!port) {
            return
        }

        setIsBusy(true)
        setErrorMessage('')

        try {
            if (port.readable || port.writable) {
                await port.close()
            }
            setPort(null)
            setStatusMessage('Disconnected')
        } catch (error) {
            setErrorMessage(error instanceof Error ? error.message : String(error))
        } finally {
            setIsBusy(false)
        }
    }

    async function updateConfig(nextSpeed, nextLanguage, nextStyle) {
        if (!port) {
            setErrorMessage('Not connected to a device.')
            return
        }

        setIsBusy(true)
        setErrorMessage('')

        try {
            await sendCommand(port, COMMANDS.SET_CONFIG, [nextSpeed, nextLanguage, nextStyle])
            setStatusMessage('Configuration updated')
        } catch (error) {
            setErrorMessage(error instanceof Error ? error.message : String(error))
        } finally {
            setIsBusy(false)
        }
    }

    async function handleSpeedChange(event) {
        const nextValue = Number(event.target.value)
        setSpeed(nextValue)
        await updateConfig(nextValue, language, style)
    }

    async function handleLanguageChange(event) {
        const nextValue = Number(event.target.value)
        setLanguage(nextValue)
        await updateConfig(speed, nextValue, style)
    }

    async function handleStyleChange(event) {
        const nextValue = Number(event.target.value)
        setStyle(nextValue)
        await updateConfig(speed, language, nextValue)
    }

    async function setCurrentTime() {
        if (!port) {
            setErrorMessage('Not connected to a device.')
            return
        }

        setIsBusy(true)
        setErrorMessage('')

        try {
            await sendCommand(port, COMMANDS.SET_TIME, getCurrentTimePayload())
            const timeData = await sendCommand(port, COMMANDS.GET_TIME)
            setWatchTime(formatWatchTime(timeData))
            setStatusMessage('Time updated')
        } catch (error) {
            setErrorMessage(error instanceof Error ? error.message : String(error))
        } finally {
            setIsBusy(false)
        }
    }

    async function sendDisplayTime() {
        if (!port) {
            setErrorMessage('Not connected to a device.')
            return
        }

        setIsBusy(true)
        setErrorMessage('')

        try {
            await sendCommand(port, COMMANDS.DISPLAY_TIME)
            setStatusMessage('Display time command sent')
        } catch (error) {
            setErrorMessage(error instanceof Error ? error.message : String(error))
        } finally {
            setIsBusy(false)
        }
    }

    return (
        <main>
            <h1>SW63 Communicator</h1>

            <p>
                <button onClick={connect} disabled={isBusy || isConnected} type="button">
                    Connect
                </button>
                {' '}
                <button onClick={disconnect} disabled={isBusy || !isConnected} type="button">
                    Disconnect
                </button>
            </p>

            <p>Status: {statusMessage}</p>
            {errorMessage ? <p>Error: {errorMessage}</p> : null}

            <section>
                <h2>Configuration</h2>

                <label htmlFor="speed-select">Speed: </label>
                <select
                    id="speed-select"
                    value={speed}
                    onChange={handleSpeedChange}
                    disabled={!isConnected || isBusy}
                >
                    {configOptions.map((value) => (
                        <option key={`speed-${value}`} value={value}>
                            {value}
                        </option>
                    ))}
                </select>

                <br />

                <label htmlFor="language-select">Language: </label>
                <select
                    id="language-select"
                    value={language}
                    onChange={handleLanguageChange}
                    disabled={!isConnected || isBusy}
                >
                    {configOptions.map((value) => (
                        <option key={`language-${value}`} value={value}>
                            {value}
                        </option>
                    ))}
                </select>

                <br />

                <label htmlFor="style-select">Style: </label>
                <select
                    id="style-select"
                    value={style}
                    onChange={handleStyleChange}
                    disabled={!isConnected || isBusy}
                >
                    {configOptions.map((value) => (
                        <option key={`style-${value}`} value={value}>
                            {value}
                        </option>
                    ))}
                </select>
            </section>

            <section>
                <h2>Time</h2>
                <p>
                    {watchTime}{' '}
                    <button onClick={setCurrentTime} disabled={!isConnected || isBusy} type="button">
                        Set Current Computer Time
                    </button>
                </p>
            </section>

            <section>
                <h2>Battery</h2>
                <p>{batteryLevel}</p>
            </section>

            <section>
                <button onClick={sendDisplayTime} disabled={!isConnected || isBusy} type="button">
                    Display Time
                </button>
            </section>
        </main>
    )
}

export default App
