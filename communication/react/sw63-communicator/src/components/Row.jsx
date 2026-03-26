import styles from './Row.module.scss';
import classNames from 'classnames';

function Row({ children, alignedHeight, style }) {
    return (
        <div
            className={classNames(styles.row, { [styles['aligned-height']]: alignedHeight })}
            style={style}
        >
            {children}
        </div>
    );
};

export default Row;