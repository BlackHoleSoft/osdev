
let commands = {
    tests: {
        label: "Run tests",
        id: 1
    },
    exit: {
        label: "Exit",
        id: 0
    }
};

function printKey() {
    _print(_state.keyboard.keycode);
    _print("; ");
    _print(_state.keyboard.keyChar);
    _print("           \n");
}

function printHeader(str) {
    _print("``````````````````````\n");
    _print(str);
    _print("\n");
}

function printCommandItem(cmd) {
    _print(cmd.id);
    _print(" ---> ");
    _print(cmd.label);
    _print("\n");
}

function runTests() {
    _print("Tests...\n");
}

function selectCommand() {
    while (true) {
        if (_state.keyboard.keycode == 0x02) {
            runTests();
            return 0;
        } else if (_state.keyboard.keycode == 0x0b) {
            _print("Exit main process.../n");
            return 0;
        }
    }
}

function start() {
    printHeader("Welcome to the Strelka - operating system written in C and Javascript ;)\n");
    _print("Select a command to run:\n\n");

    printCommandItem(commands.tests);

    _print("---------------\n");
    printCommandItem(commands.exit);

    selectCommand();
}

start();

// _print("Test state:\n");
// printKey();

// let cnt = 0;
// while (true) {
//     _printPosition(0, 6);
//     // _print(cnt);
//     // _print(': ');
//     printKey();
//     //_wait(10000000);
//     cnt = cnt + 1;
// }