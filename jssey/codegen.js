const acorn = require("acorn");
const {v4} = require("uuid");

class Parser {

    _opcodes = {
        '_literal_': 0x1,
        '_string_': 0x2,
        '_var_': 0x3,
        '_getvar_': 0x4,
        '_ret_': 0x5,
        '_call_': 0x6,
        '_mem_': 0x7,       // copy bytes to the memory and push address to stack
        // '_memupd_': 0x8,    // copy value from stack to the memory at address specified in stack [..., addr, value]
        
        '_push_': 0xa,
        '_pop_': 0xb,
        // '_stackcpy_': 0xc,  // clone last value in stack

        '_propget_': 0x10,      // push value of property to the stack, gets from stack addr of objct and addr of prop name
        '_propset_': 0x11,

        '+': 0x20,
        '-': 0x21,
        '*': 0x22,
        '/': 0x23,
        '=': 0x40,   
        
        '<': 0x50,
        '>': 0x51,
        '<=': 0x52,
        '>=': 0x53,
        '==': 0x54,
        '!=': 0x55,
        
        '_label_': 0xA0,
        '_jmpnot_': 0xA1,
        '_jmpif_': 0xA2,
        '_jmp_': 0xA3,
    }

    _varTypes = {
        'number': 0x0,
        'string': 0x1,
        'boolean': 0x2,
        'object': 0x3,
        'pointer': 0x4,
    }

    _labels = [];
    _namesDir = ['_'];
    _currentFunction = 0;
    _parseFns = {};
    _variables = [];
    _functions = ['main'];
    _systemFunctions = [];
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
            "WhileStatement": this.parseWhileStatement,
            "ForStatement": this.parseForStatement,
            "IfStatement": this.parseIfStatement,
            "ObjectExpression": this.parseObjectExpression,
            "MemberExpression": this.parseMemberExpression,
        };

        this.addSystemFn('_print');
        this.addSystemFn('_toString');
    }

    addSystemFn(name) {
        this._result.push([]);
        this._functions.push(name);
        this._systemFunctions.push(name);
    }

    getVarName = (initial) => {
        //return [...this._namesDir, initial].join('/');
        return this._currentFunction + '__' + initial;
    }

    write = (items) => {
        this._result[this._currentFunction].push(items);
    }

    addToMemory = (bytes) => {
        this.write(['_mem_', bytes.length, ...bytes.map(b => '#'+b)]);
    }    

    addLabel(uuid) {
        let lIndex = this._labels.length
        this._labels.push(uuid);
        this.write(['$'+uuid]);
        return lIndex;
    }

    toLabelAddr(label) {
        return '&'+label;
    }

    parseObjectExpression = ({properties}) => {
        let maxPropNameLength = 16;
        let objId = v4();

        let bytes = [...this.intToByteArray(properties.length)];
        properties.forEach((p) => {
            // create vars for each property
            let varIndex = this.createVariable(p.value, {
                name: objId + '.' + p.key.name
            });

            let name = p.key.name.split('').map(c => c.charCodeAt(0));
            if (name.length > maxPropNameLength)
                throw new Error("Property name too long!");
            for (let i = name.length; i < maxPropNameLength; i++) {
                name.push(0);
            }
            
            bytes.push(...[...name, 0 /* type (reserved) */, ...this.intToByteArray(varIndex)]);
        });
        this.addToMemory(bytes);

        // at the end of execution stack contains address of the object in memory
    }

    parseMemberExpression = ({object, property}) => {
        this.parseNode(object);
        this.addToMemory([...property.name.split('').map(c => c.charCodeAt(0)), 0]);
        // now stack contains address of the object and address of prop name

        this.write(['_propget_'])
    }

    parseIfStatement = ({test, consequent, alternate}) => {
        let label1 = v4();
        let label2 = v4();

        this.parseNode(test);
        this.write(['_jmpnot_', this.toLabelAddr(label1)]);

        this.parseNode(consequent);
        this.write(['_jmp_', this.toLabelAddr(label2)]);

        this.addLabel(label1);
        if (alternate) this.parseNode(alternate);
        
        this.addLabel(label2);
    }

    parseWhileStatement = ({test, body}) => {
        let label1 = v4();
        let label2 = v4();

        this.write(['_jmp_', this.toLabelAddr(label2)]);
        
        this.addLabel(label1);
        this.parseNode(body);
        
        this.addLabel(label2);
        this.parseNode(test);
        this.write(['_jmpif_', this.toLabelAddr(label1)]);
    }

    parseForStatement = ({test, init, update, body}) => {
        let label1 = v4();
        let label2 = v4();

        this.parseNode(init);
        this.write(['_jmp_', this.toLabelAddr(label2)]);
        
        this.addLabel(label1);
        this.parseNode(body);
        this.parseNode(update);
        
        this.addLabel(label2);
        this.parseNode(test);
        this.write(['_jmpif_', this.toLabelAddr(label1)]);
    }

    parseReturnStatement = ({argument}) => {
        this.parseNode(argument);
        this.write(['_ret_']);
    }

    parseFunctionDeclaration = ({id, params, body}) => {
        let fnName = id.name;
        if (this._result[fnName])
            throw new Error("Function already exists: " + fnName);

        let fnIndex = this._functions.length;
        this._functions.push(fnName);
        this._result[fnIndex] = [];
        let prevFn = this._currentFunction;
        this._currentFunction = fnIndex;

        [...params].reverse().forEach(p => {
            let index = this._variables.length;
            let varName = this.getVarName(p.name);
            this._variables.push(varName);
            this.write(['_var_', index]);
        });        

        this.parseNode(body);

        this._currentFunction = prevFn;
    }

    parseCallExpression = ({callee, arguments: args}) => {
        let isSystemFunction = this._systemFunctions.includes(callee.name);

        // TODO: call functions inside block or other function


        var fnName = callee.name;
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
            bytes.push(0);
            this.addToMemory(bytes);
        } else {
            this.write(['_push_', '#' + (this._varTypes[typeof value] || 0), +value]);
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

        // if one of operands is string then concatenete two strings
        if (operator === '+' && [left, right].some(v => v.value && typeof v.value === 'string')) {

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

    createVariable = (init, identifier) => {
        let varName = this.getVarName(identifier.name);

        this.parseNode(init);

        let index = this._variables.length;
        this._variables.push(varName);

        this.write(["_var_", index]);

        return index;
    }

    parseVariableDeclarator = ({init, id}) => {
        this.createVariable(init, id);
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
        
        let labels = {};
        let codePtr = 2 + 2;
        let startFnPtr = codePtr;

        let bytes = [
            // variables count
            ...this.shortToByteArray(this._variables.length),
            // functions count
            ...this.shortToByteArray(this._functions.length),
            // functions list
            ...this._result.map(r => {
                codePtr += 4;
                startFnPtr = codePtr;            
                let rFlat = r.flat().map(byte => {
                    if (typeof byte === 'string') {
                        if (byte.startsWith('#')) {
                            codePtr += 1;
                            return [parseInt(byte.substring(1))];
                        } else if (byte.startsWith('$')) {
                            // label declaration
                            labels[byte.substring(1)] = codePtr;
                            return [];
                        } else if (byte.startsWith('&')) {
                            // label address
                            codePtr += 8;
                            return byte;
                        } else {
                            codePtr += 1;
                            return [this._opcodes[byte]];
                        }
                    } else {
                        codePtr += 8;
                        return this.doubleToByteArray(byte);
                    }
                }).flat();
                return [...this.intToByteArray(codePtr - startFnPtr), ...rFlat]
            }).flat(),
            0, 0, 0, 0
        ];

        bytes = bytes.map(b => {
            if (typeof b === 'string' && b.startsWith('&')) {
                return this.doubleToByteArray(labels[b.substring(1)]);
            }
            return [b];
        }).flat();

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