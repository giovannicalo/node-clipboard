const EventEmitter = require("events");

const bindings = require("bindings");

const InternalClipboard = bindings("node-clipboard");

class Clipboard extends EventEmitter {

	// eslint-disable-next-line no-unused-private-class-members
	#instance = null;

	constructor() {
		super();
		this.#instance = new InternalClipboard((data) => {
			this.emit("data", data);
		});
	}

}

module.exports = new Clipboard();
