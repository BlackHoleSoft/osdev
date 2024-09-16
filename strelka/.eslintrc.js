module.exports = {
    env: {
        browser: true,
        es6: true,
        node: true,
    },
    parser: '@typescript-eslint/parser',
    extends: [
        'eslint:recommended',
        'plugin:react/recommended',
        'plugin:@typescript-eslint/recommended',
        'plugin:react-hooks/recommended',
        'plugin:prettier/recommended',
    ],
    globals: {
        module: 'readonly',
        process: 'readonly',
        JSX: 'readonly',
    },
    parserOptions: {
        ecmaFeatures: {
            jsx: true,
            legacyDecorators: true,
        },
        ecmaVersion: 2018,
        sourceType: 'module',
    },
    plugins: ['react', '@typescript-eslint', 'prettier'],
    rules: {        
        quotes: ['error', 'single'],
        semi: ['warn', 'always'],
        curly: ['error', 'multi-line'],
        'arrow-parens': [2, 'as-needed'],
        'one-var': ['error', 'never'],
        'max-len': ['error', {code: 100, ignoreStrings: true}],
        'object-curly-spacing': ['error', 'never'],
        'array-bracket-spacing': ['error', 'never'],
        'comma-style': ['error', 'last'],
        'comma-spacing': ['error', {before: false, after: true}],
        'space-infix-ops': 'error',
        'space-in-parens': ['error', 'never'],
        'space-before-function-paren': [
            'error',
            {
                anonymous: 'always',
                named: 'never',
                asyncArrow: 'always',
            },
        ],
        'arrow-spacing': 'error',
        'space-before-blocks': 'error',
        'prefer-const': 'error',
        'no-undef': 'error',        
        'react/jsx-no-target-blank': 'off',
        'react/prop-types': [warnId, {ignore: ['children']}],
        'react/react-in-jsx-scope': 'off',
        'react/jsx-curly-brace-presence': 'error',
        'react-hooks/exhaustive-deps': 'error',
        '@typescript-eslint/no-explicit-any': 'off',
        '@typescript-eslint/no-unused-vars': [
            'error',
            {
                ignoreRestSiblings: true,
                destructuredArrayIgnorePattern: '^_',
                argsIgnorePattern: '^_',
            },
        ],
    },
    settings: {
        react: {
            version: '18.2',
        },
    },    
};
