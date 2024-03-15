const fs = require('fs');

const filesTableSizeInSectors = 0xFF;
const fileHeaderNameSize = 64;
const fileHeaderTotalSize = 512;

function getHeaderSector() {
    let bytes = Array.from(Array(512)).map(el => 0);

    // Info
    ('STRELKAFSv1').split('').forEach((c, i) => bytes[i] = c.charCodeAt(0));  
    
    // Files table size in sectors
    bytes[32] = filesTableSizeInSectors;

    return bytes;
}

function getInitialFilesTable() {
    let bytes = Array.from(Array(512)).map(el => 0);

    // Test file header
    'readme.txt'.split('').forEach((c, i) => bytes[0 + i] = c.charCodeAt(0));
    
    // File info
    let infoOffset = 0 + fileHeaderNameSize;

    // Creation date    
    '2024-03-12T00:00:00'.split('').forEach((c, i) => bytes[infoOffset + i] = c.charCodeAt(0));
    infoOffset += 20;

    // Upd date
    '2024-03-12T00:00:00'.split('').forEach((c, i) => bytes[infoOffset + i] = c.charCodeAt(0));
    infoOffset += 20;

    // Size in sectors
    bytes[infoOffset + 0] = 1;
    infoOffset += 2;

    // Reserved
    infoOffset += 128 - 40 - 2;

    // Data sectors array
    bytes[infoOffset + 1] = 1; // sector 0x100

    return bytes;
}

function makefs(sectorsCount) {
    let buffer = Buffer.alloc(sectorsCount * 512);
    let data = new Uint8Array([...getHeaderSector(), ...getInitialFilesTable()]);
    buffer.fill(data, 0, data.length);

    'Strelka file system'.split('').forEach((c, i) =>  buffer[(filesTableSizeInSectors + 1) * 512 + i] = c.charCodeAt(0));

    fs.writeFileSync('disk.vhd', buffer, 'ascii');

    console.log('Disk created successfully');
}

makefs(+(process.argv[2]) || (64 * 1024));