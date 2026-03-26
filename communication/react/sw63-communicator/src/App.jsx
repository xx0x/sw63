
import classNames from 'classnames'
import { useState } from 'react'
import { useTranslation } from 'react-i18next'
import styles from './App.module.scss'
import Box from './components/Box'
import Button from './components/Button'
import Dropdown from './components/Dropdown'
import Log from './components/Log'
import Row from './components/Row'
import TickingWatchTime from './components/TickingWatchTime'
import { CONFIG_OPTIONS, SW63Client } from './SW63Client'
import Sw63Logo from './Sw63Logo'
import { getTimeNow } from './utils'

const serial_available = ('serial' in navigator);

const appLanguageOptions = [
    { value: 'en', label: 'English' },
    { value: 'cs', label: 'Čeština' }
];

function App() {

    const { t, i18n } = useTranslation()

    const [client] = useState(() => new SW63Client())
    const [isConnected, setIsConnected] = useState(false)
    const [isBusy, setIsBusy] = useState(false)
    const [errorMessage, setErrorMessage] = useState('')

    const [deviceLog, setDeviceLog] = useState('')
    const appendDeviceLog = (message, clear = false) => {
        const timeString = `${getTimeNow()} — `
        setDeviceLog((prevLog) => (clear ? '' : prevLog) + timeString + message + '\n')
    }

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
        appendDeviceLog('Connecting...', true)
        try {
            await client.connect()
            appendDeviceLog('Loading watch data...')
            await loadAllData()
            setIsConnected(true)
            appendDeviceLog('Connected')
        } catch (error) {
            await client.disconnect()
            setIsConnected(false)
            appendDeviceLog('Disconnected')
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
            appendDeviceLog('Disconnected')
        } catch (error) {
            setErrorMessage(error instanceof Error ? error.message : String(error))
        } finally {
            setIsBusy(false)
        }
    }

    async function updateConfigOption(optionIndex, optionValue, successMessage) {
        if (!isConnected) {
            return
        }
        setIsBusy(true)
        setErrorMessage('')
        try {
            await client.setConfigOption(optionIndex, optionValue)
            setConfigValue(optionIndex, optionValue)
            appendDeviceLog(successMessage)
            setTimeout(async () => {
                // Show time after config update
                await runClientCommand('displayTime', 'Display time command sent')
            }, 500)
        } catch (error) {
            if (isDeviceLostError(error)) {
                setIsConnected(false)
                appendDeviceLog('Disconnected')
            } else {
                setErrorMessage(error instanceof Error ? error.message : String(error))
            }
        } finally {
            setIsBusy(false)
        }
    }

    async function runClientCommand(methodName, successMessage, onSuccess) {
        if (!isConnected) {
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
            appendDeviceLog(successMessage)
        } catch (error) {
            if (isDeviceLostError(error)) {
                setIsConnected(false)
                appendDeviceLog('Disconnected')
            } else {
                setErrorMessage(error instanceof Error ? error.message : String(error))
            }
        } finally {
            setIsBusy(false)
        }
    }

    return (
        <main className={styles.main}>
            <div className={styles.logoContainer}>
                <Sw63Logo
                    className={classNames(styles.logo, {
                        [styles.inactive]: !isConnected
                    })}
                />
                <div
                    className={styles.appLanguage}
                >
                    {t('appLanguage')}:
                    <Dropdown
                        options={appLanguageOptions}
                        value={i18n.language}
                        onChange={(v) => i18n.changeLanguage(v)}
                    />
                </div>
            </div>
            <div className={styles.header}>
                <div>
                    <h1 className={styles.title}>
                        {t('title')}
                    </h1>
                    <div className={styles.status}>
                        <p>
                        {isConnected && <span className={styles.connected}>{t('connected')}</span>}
                            {!isConnected &&
                                <span className={styles.disconnected}>
                                    {!serial_available && t('noSerialApi')}
                                    {serial_available && t('disconnectedPrompt')}
                                </span>
                            }
                        </p>
                    </div>
                </div>
                {!isConnected && serial_available &&
                    <Button onClick={connect} disabled={isBusy} pulse>
                        {t('connect')}
                    </Button>
                }
                {isConnected && serial_available &&
                    <Button onClick={disconnect} disabled={isBusy}>
                        {t('disconnect')}
                    </Button>
                }
            </div>

            {errorMessage &&
                <div className={styles.error}>
                    <p>{t('error', { message: errorMessage })}</p>
                </div>
            }

            {isConnected &&
                <>
                    <Box
                        title={t('configBox')}
                    >
                        <Row alignedHeight style={{ marginTop: '-0.6rem' }}>
                            <span className={styles.configLabel}>{t('timeLabel')}: </span>
                            <TickingWatchTime key={watchTime} baseTime={watchTime} />
                        </Row>
                        <Row alignedHeight>
                            <label className={styles.configLabel} htmlFor="speed-select">{t('speedLabel')}: </label>
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
                        <Row alignedHeight>
                            <label className={styles.configLabel} htmlFor="language-select">{t('languageLabel')}: </label>
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
                        <Row alignedHeight>
                            <label className={styles.configLabel} htmlFor="style-select">{t('styleLabel')}: </label>
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
                        title={t('commandsBox')}
                    >
                        <Row>
                            <Button
                                onClick={() => runClientCommand('setTime', 'Time synced', setWatchTime)}
                                disabled={!isConnected || isBusy}
                            >
                                {t('syncTime')}
                            </Button>
                            <Button
                                onClick={() => runClientCommand('displayTime', 'Display time command sent')}
                                disabled={!isConnected || isBusy}
                            >
                                {t('displayTime')}
                            </Button>
                            <Button
                                onClick={() => runClientCommand('displayIntro', 'Display animation command sent')}
                                disabled={!isConnected || isBusy}
                            >
                                {t('displayAnimation')}
                            </Button>
                        </Row>
                    </Box>
                    <Box
                        title={t('deviceInfoBox')}
                    >
                        <Row>
                            <p>{t('battery')}: {batteryLevel}</p>
                            <p>{t('firmware')}: {version}</p>
                        </Row>
                    </Box>
                    <Log contents={deviceLog} />
                </>}
        </main >
    )
}

export default App
