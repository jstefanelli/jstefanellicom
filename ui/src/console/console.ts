export interface ConsoleSettings {
	grabGlobalInputs?: boolean;
	rendererSettings?: RendererSettings;
	onCommand?: (cmd: string, console: MyConsole) => void
}

export interface RendererSettings {
	prompt: string;
	promptCasses?: string;
	textClasses?: string;
	caretClasses?: string;
	caretActiveClasses?: string;
	inputClasses?: string;
}

export class ConsoleRenderer {
	protected settings: RendererSettings;
	protected promptElement: HTMLSpanElement;
	protected startElement: HTMLSpanElement;
	protected caretElement: HTMLSpanElement;
	protected endElement: HTMLSpanElement;
	protected container: HTMLDivElement;
	protected totalDelta: number = 0;

	public constructor(settings?: RendererSettings) {
		if (!settings) {
			settings = {
				prompt: "$>"
			};
		}

		if (!settings.textClasses) {
			settings.textClasses = 'text-2xl text-white whitespace-pre';
		}

		if (!settings.caretActiveClasses) {
			settings.caretActiveClasses = 'min-w-2 bg-white text-2xl whitespace-pre text-black';
		}

		if (!settings.caretClasses) {
			settings.caretClasses = 'min-w-2 text-white whitespace-pre text-2xl';
		}

		if (!settings.promptCasses) {
			settings.promptCasses = 'text-blue-500 mr-3';
		}

		this.settings = settings;

		this.container = document.createElement('div');
		this.container.classList.add('flex', 'flex-row', 'items-center');

		this.promptElement = document.createElement('span');
		if (this.settings.promptCasses) {
			this.promptElement.className = this.settings.promptCasses;
		}

		this.promptElement.innerText = this.settings.prompt;
		this.container.appendChild(this.promptElement);

		this.startElement = document.createElement('span');
		if (this.settings.textClasses) {
			this.startElement.className = this.settings.textClasses;
		}
		this.container.appendChild(this.startElement);

		this.caretElement = document.createElement('span');
		if (this.settings.caretClasses) {
			this.caretElement.className = this.settings.caretClasses;
		}
		this.container.appendChild(this.caretElement);
		this.caretElement.innerText = ' ';

		this.endElement = document.createElement('span');
		if (this.settings.textClasses) {
			this.endElement.className = this.settings.textClasses;
		}
		this.container.appendChild(this.endElement);
	}

	public attach(element: HTMLElement) {
		element.appendChild(this.container);
		this.container.scrollIntoView({
			behavior: 'auto'
		});
	}

	public renderAndDetach(value: string) {
		this.startElement.innerText = value;
		this.container.removeChild(this.caretElement);
		this.container.removeChild(this.endElement);
	}

	public render(text: string, caretOffset: number) {
		if (!text) {
			caretOffset = 0;
			text = "";
		} else {
			caretOffset = Math.min(text.length + 1, Math.max(0, caretOffset));
		}

		const stringStart = text.substring(0, text.length - caretOffset);
		let stringEnd = text.substring(text.length - caretOffset);
		if (stringEnd.length > 0) {
			this.caretElement.innerText = stringEnd.charAt(0);
			stringEnd = stringEnd.substring(1);
		} else {
			this.caretElement.innerText = ' ';
		}

		this.startElement.innerText = stringStart;
		this.endElement.innerText = stringEnd;
		this.totalDelta = 0;
	}
	
	public update(deltaSeconds: number) {
		this.totalDelta += deltaSeconds;

		this.caretElement.className = Math.floor(this.totalDelta % 2) == 0 ? 
			this.settings.caretActiveClasses : this.settings.caretClasses;
	}
}

interface KeyState {
	pressStart: number,
	lastEvent: number
}

const MaxCommandsInHistory = 30;
export default class MyConsole {
	protected container: HTMLElement;
	protected settings: ConsoleSettings;
	protected input: HTMLInputElement;
	protected currentLine: string = '';
	protected lineEditingOffset: number = 0;
	protected renderer: ConsoleRenderer;
	protected lastDeltaTime = 0;
	protected keyMap: Map<string, KeyState> = new Map<string, KeyState>();
	protected keysEnabled: boolean = true;
	protected commandHistory: string[] = [];
	protected commandHistoryPointer;

	public constructor(container: HTMLElement, settings?: ConsoleSettings) {
		this.commandHistoryPointer = -1;
		this.container = container;
		this.input = document.createElement('input');
		this.input.className = "appearance-none inline border-0 bg-transparent w-px h-px border-none outline-none overflow-hidden";
		document.body.appendChild(this.input);
		this.input.addEventListener('keypress', (ev) => { this.onKeyPress(ev); });
		this.input.addEventListener('keydown', (ev) => { this.onKeyDown(ev); });
		this.input.addEventListener('keyup', (ev) => { this.onKeyUp(ev); });
		this.input.addEventListener('input', () => { this.onInput(); });

		if (!settings) {
			settings = {};
		}

		if (settings.grabGlobalInputs === undefined) {
			settings.grabGlobalInputs = true;
		}

		this.settings = settings;

		if (this.settings.grabGlobalInputs) {
			document.body.addEventListener('focus', (ev) => { this.onFocus(ev); })
			document.body.addEventListener('click', (ev) => { this.onFocus(ev); })
		} else {
			this.container.addEventListener('focus', (ev) => { this.onFocus(ev); })
			this.container.addEventListener('click', (ev) => { this.onFocus(ev); })
		}

		this.createRenderer();
		this.onFocus(null);
		window.requestAnimationFrame((time) => { this.render(time); });
	}

	public onFocus(ev: Event) {
		this.input.focus();
	}

	public onKeyDown(ev: KeyboardEvent) {
		switch(ev.key) {
			case "Backspace":
			case "ArrowLeft":
			case "ArrowRight":
			case "ArrowUp":
			case "ArrowDown":
				this.keyMap.set(ev.key, { pressStart: performance.now(), lastEvent: 0 });
				ev.preventDefault();
				ev.stopPropagation();
				break;
		}
	}

	public onKeyUp(ev: KeyboardEvent) {
		switch(ev.key) {
			case "Backspace":
			case "ArrowLeft":
			case "ArrowRight":
			case "ArrowUp":
			case "ArrowDown":
				this.keyMap.delete(ev.key);
				ev.preventDefault();
				ev.stopPropagation();
				break;
		}
	}

	public onKeyPress(ev: KeyboardEvent) {
		if (this.keysEnabled) {
			switch(ev.key) {
				case "Enter":
					this.submitCommand(this.currentLine);
					ev.stopPropagation();
					ev.preventDefault();
					break;
				case "Tab":
					//Ignore tabs for now
					ev.stopPropagation();
					ev.preventDefault();
					break;
				case "Backspace":
					ev.stopPropagation();
					ev.preventDefault();
					break;
			}
		}
		this.input.value = '';
	}

	public onInput() {
		this.submitTextInput(this.input.value);
		this.input.value = "";
	}

	public submitTextInput(input: string) {
		if (!input || input.length == 0) {
			return;
		}

		if (this.lineEditingOffset == 0) {
			this.currentLine += input;
		} else {
			let pre = this.currentLine.substring(0, this.currentLine.length - this.lineEditingOffset);
			let pos = this.currentLine.substring(this.currentLine.length - this.lineEditingOffset);
			pre += input;

			this.currentLine = pre + pos;
		}

		this.renderer.render(this.currentLine, this.lineEditingOffset);
	}

	public backspace() {
		const offset = this.currentLine.length - this.lineEditingOffset;
		let start = this.currentLine.substring(0, offset);
		const end = this.currentLine.substring(offset);

		if (start.length > 0) {
			start = start.substring(0, start.length - 1);
		}

		this.currentLine = start + end;
		this.renderer.render(this.currentLine, this.lineEditingOffset);
	}

	public leftArrow() {
		this.lineEditingOffset += 1;
		this.renderer.render(this.currentLine, this.lineEditingOffset);
	}

	public rightArrow() {
		this.lineEditingOffset -= 1;
		if (this.lineEditingOffset < 0)
			this.lineEditingOffset = 0;
		this.renderer.render('' + this.currentLine, this.lineEditingOffset);
	}

	public upArrow() {
		if (this.commandHistoryPointer >= this.commandHistory.length - 1)
			return;
		this.commandHistoryPointer++;
		this.currentLine = this.commandHistory[this.commandHistory.length - 1 - this.commandHistoryPointer];
		this.lineEditingOffset = 0;
		this.renderer.render('' + this.currentLine, this.lineEditingOffset);
	}

	public downArrow() {
		if (this.commandHistoryPointer == 0) {
			this.currentLine = '';
			this.lineEditingOffset = 0;
			this.commandHistoryPointer = -1;
			this.renderer.render(this.currentLine, this.lineEditingOffset);
			return;
		}
		if (this.commandHistoryPointer == -1) {
			return;
		}
		this.commandHistoryPointer--;
		this.currentLine = this.commandHistory[this.commandHistory.length - 1 - this.commandHistoryPointer];
		this.lineEditingOffset = 0;
		this.renderer.render(this.currentLine, this.lineEditingOffset);
	}

	protected createRenderer() {
		this.renderer = new ConsoleRenderer(this.settings.rendererSettings);
		this.renderer.attach(this.container);
	}

	public submitCommand(cmd?: string) {
		if (cmd === undefined || cmd == null) {
			cmd = this.currentLine;
		}
		
		this.renderer.renderAndDetach(cmd);

		if (this.settings.onCommand) {
			this.settings.onCommand(cmd, this);
		}
		this.commandHistory.push(cmd);
		this.commandHistoryPointer = -1;
		if (this.commandHistory.length > MaxCommandsInHistory) {
			this.commandHistory.splice(0, this.commandHistory.length - MaxCommandsInHistory);
		}
		this.createRenderer();

		this.currentLine = "";
		this.lineEditingOffset = 0;
	}

	public clear() {
		for(let i = this.container.children.length - 1; i >= 0; i--) {
			this.container.removeChild(this.container.children.item(i));
		}
	}

	public handleKey(tm: number, key: string, action: () => void) {
		if (!this.keyMap.has(key)) {
			return;
		}
		let k = this.keyMap.get(key);
		if (k.lastEvent == 0 || (tm > k.lastEvent + 50 && tm > k.pressStart + 1000)) {
			action();
			k.lastEvent = tm;
			this.keyMap.set(key, k);
		}
		
	}

	public handleSpecialKeys() {
		const tm = performance.now();

		this.handleKey(tm, "Backspace", () => { this.backspace(); });
		this.handleKey(tm, "ArrowLeft", () => { this.leftArrow(); });
		this.handleKey(tm, "ArrowRight", () => { this.rightArrow(); });
		this.handleKey(tm, "ArrowUp", () => { this.upArrow(); });
		this.handleKey(tm, "ArrowDown", () => { this.downArrow(); });
	}

	public render(time: number) {
		if (this.keysEnabled) {
			this.handleSpecialKeys();
		}

		if (this.lastDeltaTime == 0) {
			this.lastDeltaTime = time / 1000;
		}

		const delta = (time / 1000) - this.lastDeltaTime;

		this.renderer.update(delta);
		this.lastDeltaTime = time / 1000;

		window.requestAnimationFrame((tm) => { this.render(tm); });
	}

	public appendElement(ele: HTMLElement) {
		this.container.appendChild(ele);
	}

	public printCmd(cmd: string, keyDelay?: number): Promise<MyConsole> {
		if(!keyDelay) {
			keyDelay = 30;
		}
		return new Promise<MyConsole>((res, rej) => {
			this.keysEnabled = false;
			const printNext = (i: number) => {
				if (i >= cmd.length) {
					this.keysEnabled = true;
					res(this);
					return;
				}

				let ch = cmd.charAt(i);
				this.submitTextInput(ch);
				window.setTimeout(() => {
					printNext(++i);
				}, keyDelay);
			}

			printNext(0);
		})
	}
}