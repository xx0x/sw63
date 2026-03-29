import classNames from 'classnames'
import styles from './TextButton.module.scss'

export default function TextButton({ children, translucent = false, onClick }) {
    return (
        <button
            className={classNames(styles.container, { [styles.translucent]: translucent })}
            onClick={onClick}
            role="button"
        >
            {children}
        </button>
    );
}