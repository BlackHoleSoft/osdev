import {v4 as uuid} from 'uuid';
import { Drawer } from '../fb/drawer';

export type Render = () => void;
export type ComponentFn<T> = (props: T) => Render;
export type EffectType = {
    effect: (fn: () => void, deps: any[]) => void;
    deps: any[];
}

export type ScreenContext = {
    drawer: Drawer
}
export type ComponentProps = {
    context: ScreenContext
}

export type Component<T> = {
    fn: ComponentFn<T>;
    key: string;
    state: {
        [k: string]: any
    },
    effects: EffectType[],
    render: Render;
}

const components: Component<any>[] = [];

export function renderComponents(context: ScreenContext, root: ComponentFn<ComponentProps>) {
    component("root", root, {context});
}

export function component<T>(key: string, comp: ComponentFn<T>, props: T) {
    const componentItem: Component<T> = {
        key,
        fn: comp,
        effects: [],
        state: {},
        render: () => {
            throw new Error('Empty render function in component')
        }
    };
    components.push(componentItem);    

    const renderFn = comp(props);
    componentItem.render = () => {
        renderFn();
        // TODO: effects
    }

    componentItem.render();
}

export function registerState<T>(initialValue: T): [string, Component<T>] {
    const currentComponent = components[components.length - 1];
    const id = uuid();
    currentComponent.state[id] = initialValue;

    return [id, currentComponent];
}

export function updateState<T>(id: string, value: T, comp: Component<T>) {
    comp.state[id] = value;
    comp.render();
}