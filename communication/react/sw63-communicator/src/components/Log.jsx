import { useState, useEffect, useRef, } from "react";
import styles from './Log.module.scss'

export default function Log({ contents }) {

    const logRef = useRef(null)
    const [showLog, setShowLog] = useState(false);

    useEffect(() => {
        if (!showLog || !logRef.current) {
            return
        }

        logRef.current.scrollTop = logRef.current.scrollHeight
    }, [contents, showLog])

    return (
        <>
            {showLog &&
                <pre ref={logRef} className={styles.container}>
                    {contents}
                </pre>
            }
            <div className={styles.toggleContainer}>
                {showLog &&
                    <button className={styles.toggle} onClick={() => setShowLog(false)}>
                        Hide Log
                    </button>
                }
                {!showLog &&
                    <button className={styles.toggle} onClick={() => setShowLog(true)}>
                        Show Log
                    </button>
                }
            </div>
        </>
    );
}