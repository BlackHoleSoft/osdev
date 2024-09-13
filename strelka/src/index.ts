const { createDomObject } = require("./dom");
const { render } = require("./domrender");

// clear viewport
process.stdout.write("\x1Bc");

console.log("Strelka Core running");
createDomObject();

window.test();

setTimeout(() => {
  // clear viewport
  process.stdout.write("\x1Bc");

  render(window.document.body);
}, 2000);

while (1) {}
