import type { EventEmitter } from "events";

declare interface Image {
	data: Uint8ClampedArray;
	height: number;
	width: number;
}

declare interface Events {
	data: [data: string | Image];
}

declare class Clipboard extends EventEmitter<Events> {}

declare const clipboard: Clipboard;

export = clipboard;
