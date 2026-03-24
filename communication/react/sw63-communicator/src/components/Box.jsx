import styles from './Box.module.scss';

function Box(props) {

    return (
        <div
            className={styles.container}
        >
            <div
                className={styles.header}
            >
                <div
                    className={styles.title}
                >
                    {props.title}
                </div>
            </div>
            <div
                className={styles.content}
            >
                {props.children}
            </div>
        </div>
    );
}

export default Box;