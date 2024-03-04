let a = {
    a: 6,
    b: 34,
    c: true,
    d: "test",
};
a.b = a.b + 10;
a.a = a.b + a.a;
a.d = "test2";
_print(a.a);
_print(a.b);
_print(a.d);