import { JSDOM } from "jsdom";

export const createDomObject = () => {
  const dom = new JSDOM("<body><div>Hello world</div></body>");

  global.window = dom.window;
};
