
import styles from './App.module.scss'
import { useState } from 'react'
import { CONFIG_OPTIONS, SW63Client } from './SW63Client'
import TickingWatchTime from './components/TickingWatchTime'
import Box from './components/Box'
import Button from './components/Button'
import Row from './components/Row'
import Dropdown from './components/Dropdown'

function App() {
    const [client] = useState(() => new SW63Client())
    const [isConnected, setIsConnected] = useState(false)
    const [isBusy, setIsBusy] = useState(false)
    const [errorMessage, setErrorMessage] = useState('')
    const [statusMessage, setStatusMessage] = useState('Disconnected')

    const [configOptions, setConfigOptions] = useState(() => Object.values(CONFIG_OPTIONS).map(() => []))
    const [configOptionsValues, setConfigOptionsValues] = useState(() => Object.values(CONFIG_OPTIONS).map(() => 0))

    const [watchTime, setWatchTime] = useState('N/A')
    const [batteryLevel, setBatteryLevel] = useState('N/A')
    const [version, setVersion] = useState('N/A')

    function isDeviceLostError(error) {
        const message = error instanceof Error ? error.message : String(error)
        return message.includes('The device has been lost')
    }

    function setConfigValue(optionIndex, optionValue) {
        setConfigOptionsValues((prevValues) => {
            const nextValues = [...prevValues]
            nextValues[optionIndex] = optionValue
            return nextValues
        })
    }

    async function loadAllData() {

        const optionIndices = Object.values(CONFIG_OPTIONS)

        const loadedConfigOptions = []
        const loadedConfigOptionsValues = []

        for (const i of optionIndices) {
            loadedConfigOptionsValues.push(await client.getConfigOption(i))
            loadedConfigOptions.push(await client.getConfigOptionValues(i))
        }

        setConfigOptions(loadedConfigOptions)
        setConfigOptionsValues(loadedConfigOptionsValues)

        const time = await client.getTime()
        setWatchTime(time)

        const battery = await client.getBattery()
        setBatteryLevel(String(battery) + '%')

        const ver = await client.getVersion()
        setVersion(ver)
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
            if (isDeviceLostError(error)) {
                setIsConnected(false)
                setStatusMessage('Disconnected')
            } else {
                setErrorMessage(error instanceof Error ? error.message : String(error))
            }
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
            if (isDeviceLostError(error)) {
                setIsConnected(false)
                setStatusMessage('Disconnected')
            } else {
                setErrorMessage(error instanceof Error ? error.message : String(error))
            }
        } finally {
            setIsBusy(false)
        }
    }

    return (
        <main className={styles.main}>
            <div className={styles.header}>
                <div>
                    <h1 className={styles.title}>
                        SW63 Communicator
                    </h1>
                    <div className={styles.status}>
                        <p>
                            {isConnected && <span className={styles.connected}>Status: Connected</span>}
                            {!isConnected && <span className={styles.disconnected}>Disconnected. Plug in the device and click on “Connect”.</span>}
                        </p>
                    </div>
                </div>
                {!isConnected &&
                    <Button onClick={connect} disabled={isBusy}>
                        Connect
                    </Button>
                }
                {isConnected &&
                    <Button onClick={disconnect} disabled={isBusy}>
                        Disconnect
                    </Button>
                }
            </div>


            {/* <p>Status: {statusMessage}</p> */}
            {errorMessage &&
                <div className={styles.error}>
                    <p>Error: {errorMessage}</p>
                </div>
            }


            {isConnected &&
                <>
                    <Box
                        title="Configuration"
                    >
                        <Row>
                            <span className={styles.configLabel}>Time: </span>
                            <TickingWatchTime key={watchTime} baseTime={watchTime} />


                        </Row>
                        <Row>
                            <label className={styles.configLabel} htmlFor="speed-select">Speed: </label>
                            <Dropdown
                                id="speed-select"
                                value={configOptionsValues[CONFIG_OPTIONS.SPEED]}
                                onChange={(v) => updateConfigOption(CONFIG_OPTIONS.SPEED, Number(v), 'Speed updated')}
                                disabled={!isConnected || isBusy}
                                options={configOptions[CONFIG_OPTIONS.SPEED].map((name, index) => ({
                                    value: index,
                                    label: parseInt(name, 10) + 1
                                }))}
                            />
                        </Row>
                        <Row>
                            <label className={styles.configLabel} htmlFor="language-select">Language: </label>
                            <Dropdown
                                id="language-select"
                                value={configOptionsValues[CONFIG_OPTIONS.LANGUAGE]}
                                onChange={(v) => updateConfigOption(CONFIG_OPTIONS.LANGUAGE, Number(v), 'Language updated')}
                                disabled={!isConnected || isBusy}
                                options={configOptions[CONFIG_OPTIONS.LANGUAGE].map((name, index) => ({
                                    value: index,
                                    label: name
                                }))}
                            />
                        </Row>
                        <Row>
                            <label className={styles.configLabel} htmlFor="style-select">Style: </label>
                            <Dropdown
                                id="style-select"
                                value={configOptionsValues[CONFIG_OPTIONS.STYLE]}
                                onChange={(v) => updateConfigOption(CONFIG_OPTIONS.STYLE, Number(v), 'Style updated')}
                                disabled={!isConnected || isBusy}
                                options={configOptions[CONFIG_OPTIONS.STYLE].map((name, index) => ({
                                    value: index,
                                    label: name
                                }))}
                            />
                        </Row>
                    </Box>
                    <Box
                        title="Commands"
                    >
                        <Row>
                            <Button
                                onClick={() => runClientCommand('setTime', 'Time updated', setWatchTime)}
                                disabled={!isConnected || isBusy}
                            >
                                Update time from computer
                            </Button>
                            <Button
                                onClick={() => runClientCommand('displayTime', 'Display time command sent')}
                                disabled={!isConnected || isBusy}
                            >
                                Display Time
                            </Button>
                            <Button
                                onClick={() => runClientCommand('displayIntro', 'Display intro command sent')}
                                disabled={!isConnected || isBusy}
                            >
                                Display Animation
                            </Button>
                        </Row>
                    </Box>



                    <Box
                        title="Device Info"
                    >
                        <Row>
                            <p>Battery: {batteryLevel}</p>
                            <p>Firmware: {version}</p>
                        </Row>
                    </Box>
                </>}
        </main >
    )
}

export default App
