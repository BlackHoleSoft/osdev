const acorn = require("acorn");

class Parser {

    _opcodes = {
        '_literal_': 0x1,
        '_string_': 0x2,
        
        '_push_': 0xa,
        '_pop_': 0xb,

        '+': 0x20,
        '-': 0x21,
        '*': 0x22,
        '/': 0x23,
        '=': 0x40,        
    }

    _result = [];

    parseLiteral({value}) {
        if (typeof value === 'string') {
            
        } else {
            this._result.push(['_push_', value]);
        }
    }

    parseBinaryExpression({left, right, operator}) {
        let oper = this._opcodes[operator];
        if (!oper) {
            throw new Error("Invalid operator: " + operator);
        }

        this.parseExpression(left);
        this.parseExpression(right);

        this._result.push([operator]);
    }

    parseExpression(node) {
        if (node.type === "BinaryExpression") {
            this.parseBinaryExpression(node);
        }
        if (node.type === "Literal") {
            this.parseLiteral(node);
        }
    }

    parseNode(node) {
        if (node.type === "ExpressionStatement") {
            this.parseExpression(node.expression);
        }
    }

    parseBody(body) {
        body.forEach(node => {
            this.parseNode(node);
        });
        return this._result;
    }

    reset() {
        this._result = [];
    }
}

class Codegen {
    _config = {
        ecmaVer: 2020
    };

    

    generate(code) {
        const tree = acorn.parse(code, {ecmaVersion: this._config.ecmaVer});

        const treeParser = new Parser();
        const operations = treeParser.parseBody(tree.body);

        console.log(operations);
    }
}

module.exports = Codegen;