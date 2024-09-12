
export const createDomObject = () => {
    global.window = {
        test: () => console.log('Window works')
    }
}