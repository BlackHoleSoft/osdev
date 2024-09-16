const {app, BrowserWindow} = require('electron');
const path = require('path');

const dev = !app.isPackaged;
console.log('Is Packaged:', app.isPackaged);

const createWindow = () => {
    // Create the browser window.
    const mainWindow = new BrowserWindow({
        width: 800,
        height: 600,
        fullscreen: false,
        maximized: true,
        alwaysOnTop: false,
        frame: dev,
        webPreferences: {
            preload: path.join(__dirname, 'preload.js'),
            devTools: true,
        },
    });
    mainWindow.maximize();

    // and load the index.html of the app.
    mainWindow.loadFile('bundle/strelka.html');

    // Open the DevTools.
    if (dev) mainWindow.webContents.openDevTools();

    // const testAppWnd = new BrowserWindow({
    //     width: 400,
    //     height: 400,
    //     alwaysOnTop: true,
    //     frame: false,
    //     parent: mainWindow,
    // });
    // testAppWnd.loadURL('https://ya.ru/');
};

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.whenReady().then(() => {
    createWindow();

    app.on('activate', () => {
        // On macOS it's common to re-create a window in the app when the
        // dock icon is clicked and there are no other windows open.
        if (BrowserWindow.getAllWindows().length === 0) createWindow();
    });
});

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', () => {
    if (process.platform !== 'darwin') app.quit();
});
