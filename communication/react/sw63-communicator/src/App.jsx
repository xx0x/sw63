
import { useMemo, useState } from 'react'
import { SW63Client } from './SW63Client'

function App() {
    const [client] = useState(() => new SW63Client())
    const [isConnected, setIsConnected] = useState(false)
    const [isBusy, setIsBusy] = useState(false)
    const [errorMessage, setErrorMessage] = useState('')
    const [statusMessage, setStatusMessage] = useState('Disconnected')

    const [speed, setSpeed] = useState(0)
    const [language, setLanguage] = useState(0)
    const [style, setStyle] = useState(0)
    const [watchTime, setWatchTime] = useState('N/A')
    const [batteryLevel, setBatteryLevel] = useState('N/A')

    const configOptions = useMemo(() => [0, 1, 2, 3, 4], [])

    async function loadAllData() {
        const config = await client.getConfig()
        setSpeed(config.speed)
        setLanguage(config.language)
        setStyle(config.style)

        const time = await client.getTime()
        setWatchTime(time)

        const battery = await client.getBattery()
        setBatteryLevel(String(battery))
    }

    async function connect() {
        setIsBusy(true)
        setErrorMessage('')
        setStatusMessage('Connecting...')

        try {
            await client.connect()
            setStatusMessage('Connected. Loading watch data...')

            await loadAllData()
            setIsConnected(true)
            setStatusMessage('Connected')
        } catch (error) {
            await client.disconnect()
            setIsConnected(false)
            setStatusMessage('Disconnected')
            setErrorMessage(error instanceof Error ? error.message : String(error))
        } finally {
            setIsBusy(false)
        }
    }

    async function disconnect() {
        setIsBusy(true)
        setErrorMessage('')

        try {
            await client.disconnect()
            setIsConnected(false)
            setStatusMessage('Disconnected')
        } catch (error) {
            setErrorMessage(error instanceof Error ? error.message : String(error))
        } finally {
            setIsBusy(false)
        }
    }

    async function updateConfig(nextSpeed, nextLanguage, nextStyle) {
        if (!isConnected) {
            setErrorMessage('Not connected to a device.')
            return
        }

        setIsBusy(true)
        setErrorMessage('')

        try {
            await client.setConfig(nextSpeed, nextLanguage, nextStyle)
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
        if (!isConnected) {
            setErrorMessage('Not connected to a device.')
            return
        }

        setIsBusy(true)
        setErrorMessage('')

        try {
            const time = await client.setTime()
            setWatchTime(time)
            setStatusMessage('Time updated')
        } catch (error) {
            setErrorMessage(error instanceof Error ? error.message : String(error))
        } finally {
            setIsBusy(false)
        }
    }

    async function sendDisplayTime() {
        if (!isConnected) {
            setErrorMessage('Not connected to a device.')
            return
        }

        setIsBusy(true)
        setErrorMessage('')

        try {
            await client.displayTime()
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
