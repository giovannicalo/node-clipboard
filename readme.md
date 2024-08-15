# Node Clipboard

## Prerequisites

* [CMake](https://cmake.org) >= 3.21.0

> Currently only implemented for Windows.

## Installation

```bash
npm install giovannicalo/node-clipboard
```

> Not yet published to NPM. This will install it from GitHub.

## Usage

```javascript
const clipboard = require("clipboard");

clipboard.on("data", (data) => {
    console.log(data);
});
```

## API

### `clipboard: Clipboard`

Clipboard object. The `Clipboard` class extends [`EventEmitter`](https://nodejs.org/api/events.html#class-eventemitter), so it has access to all of its methods.

#### Events

##### `data` - `(data: string | Image): void`

Emitted when the clipboard is updated. It provides either a `string` if text was copied or an `Image` object with `data`, `height` and `width` properties if an image was. The image's data is in RGBA format.
