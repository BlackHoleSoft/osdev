import { registerState, updateState } from "./component";

export function state<T>(initialValue: T) {
    const [id, component] = registerState(initialValue);

    function setValue(value: T) {
        updateState(id, value, component);
    }

    return [
        component.state[id],
        setValue
    ];
}