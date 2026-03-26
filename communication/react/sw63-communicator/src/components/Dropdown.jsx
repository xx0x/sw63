import classNames from 'classnames';
import style from './Dropdown.module.scss';

export default function Dropdown(props) {

    return (
        <select
            className={classNames({
                [style.container]: true,
                [style.disabled]: props.disabled
            }, props.className)}
            id={props.id}
            disabled={props.disabled}
            value={props.value}
            onChange={(e) => props.onChange(e.target.options[e.target.selectedIndex].value)}
        >
            {props.options.map(({ value, label }) => (
                <option key={value} value={value}>{label}</option>
            ))}
        </select>
    );
}