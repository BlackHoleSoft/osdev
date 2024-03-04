function pow(v, p) {
    let result = v;
    for (let i = 1; i < p; i = i + 1) {
        result = result * v;
    }
    return result;
}

function getCylynderVolume(r, h) {
    return 3.14 * pow(r, 2) * h;
}

_print("Engine volume in cm3:");
_print(getCylynderVolume(56, 50) * 4 / 1000);


