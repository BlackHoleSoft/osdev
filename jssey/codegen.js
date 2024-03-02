const acorn = require("acorn");

class Parser {

    _opcodes = {
        '_literal_': 0x1,
        '_string_': 0x2,
        '_var_': 0x3,
        '_getvar_': 0x4,
        '_ret_': 0x5,
        '_call_': 0x6,
        '_mem_': 0x7,   // copy bytes to the memory and push address to stack
        
        '_push_': 0xa,
        '_pop_': 0xb,

        '+': 0x20,
        '-': 0x21,
        '*': 0x22,
        '/': 0x23,
        '=': 0x40,        
    }

    _namesDir = ['_'];
    _currentFunction = 0;
    _parseFns = {};
    _variables = [];
    _functions = ['main'];
    _result = [
        [],  //main
    ];

    constructor () {
        this._parseFns = {
            "ExpressionStatement": this.parseExpressionStatement,
            "BinaryExpression": this.parseBinaryExpression,
            "Literal": this.parseLiteral,
            "VariableDeclarator": this.parseVariableDeclarator,
            "VariableDeclaration": this.parseVariableDeclaration,
            "Identifier": this.parseIdentifier,
            "AssignmentExpression": this.parseAssignmentExpression,
            "BlockStatement": this.parseBlockStatement,
            "FunctionDeclaration": this.parseFunctionDeclaration,
            "ReturnStatement": this.parseReturnStatement,
            "CallExpression": this.parseCallExpression,
        };

        this.addSystemFn('_print');
    }

    addSystemFn(name) {
        this._result.push([]);
        this._functions.push('_/' + name);
    }

    getVarName = (initial) => {
        return [...this._namesDir, initial].join('/');
    }

    write = (items) => {
        this._result[this._currentFunction].push(items);
    }

    addToMemory = (bytes) => {
        this.write(['_mem_', bytes.length, ...bytes.map(b => '#'+b)]);
    }

    parseReturnStatement = ({argument}) => {
        this.parseNode(argument);
        this.write(['_ret_']);
    }

    parseFunctionDeclaration = ({id, params, body}) => {
        let fnName = this.getVarName(id.name);
        if (this._result[fnName])
            throw new Error("Function already exists: " + fnName);

        let fnIndex = this._functions.length;
        this._functions.push(fnName);
        this._result[fnIndex] = [];
        let prevFn = this._currentFunction;
        this._currentFunction = fnIndex;

        [...params].reverse().forEach(p => {
            let index = this._variables.length;
            let varName = [...this._namesDir, `fn${fnIndex}_block`, p.name].join('/');
            this._variables.push(varName);
            this.write(['_var_', index]);
        });        

        this.parseNode(body);

        this._currentFunction = prevFn;
    }

    parseCallExpression = ({callee, arguments: args}) => {
        var fnName = this.getVarName(callee.name);
        var fnIndex = this._functions.indexOf(fnName);
        if (fnIndex < 0)
            throw new Error("Function was not initialized: " + fnName);

        args.forEach(a => {
            this.parseNode(a);
        });

        this.write(['_call_', fnIndex]);
    }

    parseBlockStatement = ({body}) => {
        this._namesDir.push(`fn${this._currentFunction}_block`);
        body.forEach(node => {
            this.parseNode(node);
        });
        this._namesDir.pop();
    }

    parseLiteral = ({value}) => {
        if (typeof value === 'string') {
            let bytes = value.split('').map(c => c.charCodeAt(0) & 0xFF);
            this.addToMemory(bytes);
        } else {
            this.write(['_push_', value]);
        }
    }

    parseIdentifier = ({name}) => {
        let varName = this.getVarName(name);
        let varIndex = this._variables.indexOf(varName);

        if (varIndex < 0)
            throw new Error('Variable was not initialized: ' + varName)

        this.write(['_getvar_', varIndex]);
    }

    parseExpressionStatement = (node) => {
        this.parseNode(node.expression);
    }

    parseBinaryExpression = ({left, right, operator}) => {
        let oper = this._opcodes[operator];
        if (!oper) {
            throw new Error("Invalid operator: " + operator);
        }

        this.parseNode(left);
        this.parseNode(right);

        this.write([operator]);
    }

    parseAssignmentExpression = ({operator, left, right}) => {
        if (operator === '=') {
            let varName = this.getVarName(left.name);
            let varIndex = this._variables.indexOf(varName);

            if (varIndex < 0)
                throw new Error("Variable was not initialized: " + varName);

            this.parseNode(right);
            this.write(['_var_', varIndex]);
        } else {
            throw new Error("Invalid assignment operator: ", operator);
        }
    }

    parseVariableDeclaration = ({declarations}) => {
        declarations.forEach(d => this.parseNode(d));
    }

    parseVariableDeclarator = ({init, id}) => {
        let varName = this.getVarName(id.name);

        this.parseNode(init);

        let index = this._variables.length;
        this._variables.push(varName);

        this.write(["_var_", index]);
    }
    
    parseNode = (node) => {
        if (!this._parseFns[node.type]) {
            throw new Error("Invalid node type: " + node.type);
        }

        this._parseFns[node.type](node);
    }

    parseBody = (body) => {
        body.forEach(node => {
            this.parseNode(node);
        });

        this._result[0].push(['_ret_']);

        console.log('Variables:', this._variables);
        console.log('Functions:', this._functions);

        return this._result;
    }

    reset = () => {
        this._result = {
            main: []
        };
    }

    doubleToByteArray = (number) => {
        let buffer = new ArrayBuffer(8);         // JS numbers are 8 bytes long, or 64 bits
        let longNum = new Float64Array(buffer);  // so equivalent to Float64
    
        longNum[0] = number;
    
        return Array.from(new Int8Array(buffer));  // reverse to get little endian
    }

    shortToByteArray = (number) => {
        let buffer = new ArrayBuffer(2);         // JS numbers are 8 bytes long, or 64 bits
        let num = new Int16Array(buffer);  // so equivalent to Float64
    
        num[0] = number;
    
        return Array.from(new Int8Array(buffer));  // reverse to get little endian
    }

    intToByteArray = (number) => {
        let buffer = new ArrayBuffer(4);         // JS numbers are 8 bytes long, or 64 bits
        let num = new Int32Array(buffer);  // so equivalent to Float64
    
        num[0] = number;
    
        return Array.from(new Int8Array(buffer));  // reverse to get little endian
    }

    getByteCode = () => {
        let bytes = [
            // variables count
            ...this.shortToByteArray(this._variables.length),
            // functions count
            ...this.shortToByteArray(this._functions.length),
            // functions list
            ...this._result.map(r => {
                let rFlat = r.flat().map(byte => typeof byte === 'string' ? 
                byte.startsWith('#') ? [parseInt(byte.substring(1))] : [this._opcodes[byte]] 
                    : this.doubleToByteArray(byte)).flat();
                return [...this.intToByteArray(rFlat.length), ...rFlat]
            }).flat(),
            0, 0, 0, 0
        ];

        for (let i = bytes.length % 512; i < 512; i++) {
            bytes.push(0);
        }

        return bytes;
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

        let bytecode = treeParser.getByteCode();        

        console.log('Bytes:');
        console.log(bytecode);

        return bytecode;
    }
}

module.exports = Codegen;