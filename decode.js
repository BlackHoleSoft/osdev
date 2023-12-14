const fs = require('fs');

let data = process.argv[2];

function getBinary(base64) {
    let arr = [...Buffer.from(base64, 'base64')];
    let buf = {
        arr,
        length: arr.length
    };
    buf.gb = function (i) {
        return '0x' + this.arr[i].toString(16);
    };
    buf.gb.bind(buf);

    buf.int = function (pos, length) {
        let str = "";
        for (let i=0; i<length; i++) {
            str += this.arr[pos + i].toString(16);
        }
        return Number.parseInt("0x" + str);
    }
    buf.int.bind(buf);

    return buf;
}

if (!data) {
    console.error("Invalid arguments");
} else {
    let bin = getBinary(data);

    fs.writeFile("./decoded.txt", JSON.stringify(bin.arr), (err) => {
        if (err) {
            throw new Error("Could not write decoded data");
        }
        console.log("Decoded successfully");
    })
}