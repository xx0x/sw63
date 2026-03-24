import { useEffect, useState } from 'react'

function pad2(value) {
    return String(value).padStart(2, '0')
}

function formatDateTime(date) {
    return `${date.getFullYear()}-${pad2(date.getMonth() + 1)}-${pad2(date.getDate())} ${pad2(date.getHours())}:${pad2(date.getMinutes())}:${pad2(date.getSeconds())}`
}

function parseProtocolTime(value) {
    const match = /^([0-9]{4})-([0-9]{2})-([0-9]{2}) ([0-9]{2}):([0-9]{2}):([0-9]{2})$/.exec(value)
    if (!match) {
        return null
    }

    const year = Number(match[1])
    const month = Number(match[2])
    const day = Number(match[3])
    const hour = Number(match[4])
    const minute = Number(match[5])
    const second = Number(match[6])
    const parsed = new Date(year, month - 1, day, hour, minute, second)

    if (Number.isNaN(parsed.getTime())) {
        return null
    }

    return parsed
}

function TickingWatchTime({ baseTime }) {
    const [secondsElapsed, setSecondsElapsed] = useState(0)
    const parsedBaseTime = parseProtocolTime(baseTime)

    useEffect(() => {
        if (!parsedBaseTime) {
            return undefined
        }

        const intervalId = setInterval(() => {
            setSecondsElapsed((value) => value + 1)
        }, 1000)

        return () => clearInterval(intervalId)
    }, [parsedBaseTime])

    if (!parsedBaseTime) {
        return <>{baseTime}</>
    }

    const current = new Date(parsedBaseTime.getTime() + secondsElapsed * 1000)
    const displayTime = formatDateTime(current)

    return <span style={{display: 'inline-block', minWidth: '11rem'}}>{displayTime}</span>
}

export default TickingWatchTime
