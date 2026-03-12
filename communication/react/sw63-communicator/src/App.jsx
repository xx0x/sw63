
import { useState } from 'react'
import { SW63Client } from './SW63Client'
import TickingWatchTime from './components/TickingWatchTime'

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
    const [version, setVersion] = useState('N/A')

    const [speedOptions, setSpeedOptions] = useState([])
    const [languageOptions, setLanguageOptions] = useState([])
    const [styleOptions, setStyleOptions] = useState([])

    async function loadAllData() {
        const config = await client.getConfig()
        setSpeed(config.speed)
        setLanguage(config.language)
        setStyle(config.style)

        const time = await client.getTime()
        setWatchTime(time)

        const battery = await client.getBattery()
        setBatteryLevel(String(battery))

        // Load available options for each config field
        try {
            const speeds = await client.getConfigOptions(0)
            setSpeedOptions(speeds)
        } catch (error) {
            console.warn('Failed to load speed options:', error)
        }

        try {
            const languages = await client.getConfigOptions(1)
            setLanguageOptions(languages)
        } catch (error) {
            console.warn('Failed to load language options:', error)
        }

        try {
            const styles = await client.getConfigOptions(2)
            setStyleOptions(styles)
        } catch (error) {
            console.warn('Failed to load style options:', error)
        }

        try {
            const ver = await client.getVersion()
            setVersion(ver)
        } catch (error) {
            console.warn('Failed to load version:', error)
        }
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

            setTimeout(async () => {
                // Show time after config update
                await sendDisplayTime();
            }, 500);

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

            {isConnected &&
                <>
                    <section>
                        <h2>Configuration</h2>

                        <label htmlFor="speed-select">Speed: </label>
                        <select
                            id="speed-select"
                            value={speed}
                            onChange={handleSpeedChange}
                            disabled={!isConnected || isBusy}
                        >
                            {speedOptions.map((name, index) => (
                                <option key={`speed-${index}`} value={index}>
                                    {name}
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
                            {languageOptions.map((name, index) => (
                                <option key={`language-${index}`} value={index}>
                                    {name}
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
                            {styleOptions.map((name, index) => (
                                <option key={`style-${index}`} value={index}>
                                    {name}
                                </option>
                            ))}
                        </select>
                    </section>

                    <section>
                        <h2>Time</h2>
                        <p>
                            <TickingWatchTime key={watchTime} baseTime={watchTime} />{' '}
                            <button onClick={setCurrentTime} disabled={!isConnected || isBusy} type="button">
                                Set Current Computer Time
                            </button>
                        </p>
                        <p>
                            <button onClick={sendDisplayTime} disabled={!isConnected || isBusy} type="button">
                                Display Time
                            </button>
                        </p>
                    </section>

                    <section>
                        <h2>Battery</h2>
                        <p>{batteryLevel}</p>
                    </section>

                    <section>
                        <h2>Firmware</h2>
                        <p>Version: {version}</p>
                    </section>
                </>}
        </main>
    )
}

export default App
