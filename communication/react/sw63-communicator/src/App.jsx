
import { useState } from 'react'
import { CONFIG_OPTIONS, SW63Client } from './SW63Client'
import TickingWatchTime from './components/TickingWatchTime'

function App() {
    const [client] = useState(() => new SW63Client())
    const [isConnected, setIsConnected] = useState(false)
    const [isBusy, setIsBusy] = useState(false)
    const [errorMessage, setErrorMessage] = useState('')
    const [statusMessage, setStatusMessage] = useState('Disconnected')

    const [configOptions, setConfigOptions] = useState(() => Object.values(CONFIG_OPTIONS).map(() => []))
    const [configValues, setConfigValues] = useState(() => Object.values(CONFIG_OPTIONS).map(() => 0))

    const [watchTime, setWatchTime] = useState('N/A')
    const [batteryLevel, setBatteryLevel] = useState('N/A')
    const [version, setVersion] = useState('N/A')


    function setConfigValue(optionIndex, optionValue) {
        setConfigValues((prevValues) => {
            const nextValues = [...prevValues]
            nextValues[optionIndex] = optionValue
            return nextValues
        })
    }

    async function loadAllData() {
        const optionIndices = Object.values(CONFIG_OPTIONS)
        const loadedConfigValues = []
        for (const optionIndex of optionIndices) {
            loadedConfigValues.push(await client.getConfigOption(optionIndex))
        }
        setConfigValues(loadedConfigValues)

        const time = await client.getTime()
        setWatchTime(time)

        const battery = await client.getBattery()
        setBatteryLevel(String(battery) + '%')

        const nextConfigOptions = optionIndices.map(() => [])
        for (let i = 0; i < optionIndices.length; i++) {
            try {
                nextConfigOptions[i] = await client.getConfigOptionValues(optionIndices[i])
            } catch (error) {
                console.warn(`Failed to load #${optionIndices[i]} options:`, error)
            }
        }
        setConfigOptions(nextConfigOptions)

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

    async function updateConfigOption(optionIndex, optionValue, successMessage) {
        if (!isConnected) {
            setErrorMessage('Not connected to a device.')
            return
        }

        setIsBusy(true)
        setErrorMessage('')

        try {
            await client.setConfigOption(optionIndex, optionValue)
            setConfigValue(optionIndex, optionValue)
            setStatusMessage(successMessage)
            setTimeout(async () => {
                // Show time after config update
                await runClientCommand('displayTime', 'Display time command sent')
            }, 500)
        } catch (error) {
            setErrorMessage(error instanceof Error ? error.message : String(error))
        } finally {
            setIsBusy(false)
        }
    }

    async function runClientCommand(methodName, successMessage, onSuccess) {
        if (!isConnected) {
            setErrorMessage('Not connected to a device.')
            return
        }

        setIsBusy(true)
        setErrorMessage('')

        try {
            const clientMethod = client[methodName]
            if (typeof clientMethod !== 'function') {
                throw new Error(`Client method not found: ${methodName}`)
            }
            const result = await clientMethod.call(client)
            if (onSuccess) {
                onSuccess(result)
            }
            setStatusMessage(successMessage)
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
                            value={configValues[CONFIG_OPTIONS.SPEED]}
                            onChange={(e) => updateConfigOption(CONFIG_OPTIONS.SPEED, Number(e.target.value), 'Speed updated')}
                            disabled={!isConnected || isBusy}
                        >
                            {configOptions[CONFIG_OPTIONS.SPEED].map((name, index) => (
                                <option key={`speed-${index}`} value={index}>
                                    {parseInt(name, 10) + 1}
                                </option>
                            ))}
                        </select>

                        <br />

                        <label htmlFor="language-select">Language: </label>
                        <select
                            id="language-select"
                            value={configValues[CONFIG_OPTIONS.LANGUAGE]}
                            onChange={(e) => updateConfigOption(CONFIG_OPTIONS.LANGUAGE, Number(e.target.value), 'Language updated')}
                            disabled={!isConnected || isBusy}
                        >
                            {configOptions[CONFIG_OPTIONS.LANGUAGE].map((name, index) => (
                                <option key={`language-${index}`} value={index}>
                                    {name}
                                </option>
                            ))}
                        </select>

                        <br />

                        <label htmlFor="style-select">Style: </label>
                        <select
                            id="style-select"
                            value={configValues[CONFIG_OPTIONS.STYLE]}
                            onChange={(e) => updateConfigOption(CONFIG_OPTIONS.STYLE, Number(e.target.value), 'Style updated')}
                            disabled={!isConnected || isBusy}
                        >
                            {configOptions[CONFIG_OPTIONS.STYLE].map((name, index) => (
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
                            <button
                                onClick={() => runClientCommand('setTime', 'Time updated', setWatchTime)}
                                disabled={!isConnected || isBusy}
                                type="button"
                            >
                                Set Current Computer Time
                            </button>
                        </p>
                        <p>
                            <button
                                onClick={() => runClientCommand('displayTime', 'Display time command sent')}
                                disabled={!isConnected || isBusy}
                                type="button"
                            >
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
                    <section>
                        <button
                            onClick={() => runClientCommand('displayIntro', 'Display intro command sent')}
                            disabled={!isConnected || isBusy}
                            type="button"
                        >
                            Display Intro
                        </button>
                    </section>
                </>}
        </main>
    )
}

export default App
