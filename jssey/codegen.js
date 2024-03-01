const acorn = require("acorn");

class Parser {

    _opcodes = {
        '_literal_': 0x1,
        '_string_': 0x2,
        '_var_': 0x3,
        '_getvar_': 0x4,
        
        '_push_': 0xa,
        '_pop_': 0xb,

        '+': 0x20,
        '-': 0x21,
        '*': 0x22,
        '/': 0x23,
        '=': 0x40,        
    }

    _parseFns = {};
    _variables = [];
    _result = [];

    constructor () {
        this._parseFns = {
            "ExpressionStatement": this.parseExpressionStatement,
            "BinaryExpression": this.parseBinaryExpression,
            "Literal": this.parseLiteral,
            "VariableDeclarator": this.parseVariableDeclarator,
            "VariableDeclaration": this.parseVariableDeclaration,
            "Identifier": this.parseIdentifier,
            "AssignmentExpression": this.parseAssignmentExpression,
        };
    }


    parseLiteral = ({value}) => {
        if (typeof value === 'string') {
            
        } else {
            this._result.push(['_push_', value]);
        }
    }

    parseIdentifier = ({name}) => {
        let varIndex = this._variables.indexOf(name);
        this._result.push(['_getvar_', varIndex]);
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

        this._result.push([operator]);
    }

    parseAssignmentExpression = ({operator, left, right}) => {
        if (operator === '=') {
            let varName = left.name;
            let varIndex = this._variables.indexOf(varName);

            if (varIndex < 0)
                throw new Error("Variable was not initialized: " + varName);

            this.parseNode(right);
            this._result.push(['_var_', varIndex]);
        } else {
            throw new Error("Invalid assignment operator: ", operator);
        }
    }

    parseVariableDeclaration = ({declarations}) => {
        declarations.forEach(d => this.parseNode(d));
    }

    parseVariableDeclarator = ({init, id}) => {
        let varName = id.name;

        this.parseNode(init);

        let index = this._variables.length;
        this._variables.push(varName);

        this._result.push(["_var_", index]);
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
        return this._result;
    }

    reset = () => {
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