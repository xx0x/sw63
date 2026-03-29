import { useState, useEffect, useRef, } from "react";
import { useTranslation } from 'react-i18next'
import styles from './Log.module.scss'
import TextButton from "./TextButton";

export default function Log({ contents }) {

    const { t } = useTranslation()

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
                    <TextButton translucent onClick={() => setShowLog(false)}>
                        {t('hideLog')}
                    </TextButton>
                }
                {!showLog &&
                    <TextButton translucent onClick={() => setShowLog(true)}>
                        {t('showLog')}
                    </TextButton>
                }
            </div>
        </>
    );
}