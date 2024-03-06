
function printKey() {
    _print(_state.keyboard.keycode);
    _print("; ");
    _print(_state.keyboard.keyChar);
    _print("           \n");
}

_print("Test state:\n");
printKey();

let cnt = 0;
while (true) {
    _printPosition(0, 6);
    _print(cnt);
    _print(': ');
    printKey();
    _wait(10000000);
    cnt = cnt + 1;
}