let a = 4;
if (a > 3) {
    a = a + 10; 
}
_print(a);

let b = 3;
if (b > 100) {
    b = 1000;
}
_print(b);

let c = 6;
if (c == 6) {
    c = c * 10;
} else {
    c = 1;
}
_print(c);

let d = 7;
if (d < 0) {
    d = 1;
} else if (d > 5) {
    d = 100;
} else {
    d = 1000;
}
_print(d);

let e = 30;
if (e > 10) {
    e = e * e;
} else if (e < 0) {
    e = 80;
}
_print(e);