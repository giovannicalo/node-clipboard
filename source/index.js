const EventEmitter = require("events");

const bindings = require("bindings");

const InternalClipboard = bindings("node-clipboard");

class Clipboard extends EventEmitter {

	#instance = null;

	constructor() {
		super();
		this.on("newListener", this.#start);
		this.on("removeListener", this.#stop);
	}

	#emit = (data) => {
		this.emit("data", data);
	};

	#start(event) {
		if (this.#instance || event !== "data") {
			return;
		}
		this.#instance = new InternalClipboard(this.#emit);
	}

	#stop(event) {
		if (event !== "data" || this.listenerCount("data")) {
			return;
		}
		this.stop();
	}

	stop() {
		this.#instance?.stop();
		this.#instance = null;
	}

}

const clipboard = new Clipboard();

module.exports = clipboard;
