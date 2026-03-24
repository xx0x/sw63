import classNames from 'classnames';
import style from './Button.module.scss';

function Button({
    tagName: TagName = 'button',
    onClick,
    primary,
    secondary,
    disabled,
    children
}) {
    return (
        <TagName
            type={TagName === 'button' ? 'button' : null}
            onClick={onClick}
            className={classNames({
                [style.container]: true,
                [style.primary]: primary,
                [style.secondary]: secondary,
                [style.disabled]: disabled,
            })}
            disabled={disabled}
        >
            {children}
        </TagName>
    );
}

export default Button;