import './style.css';
import MyConsole from './console/console';
import CommandParser from './console/commandParser';

class Misfortunes {
	protected static safeMisfortunes: string[] = [
		"\"I am me\" - Jake from LesserKnownMedia",
		"*vomiting sounds* - Mark Webber, 2007 Fuji GP",
		"\"Bwoah.\" - Kimi Räikkönen",
		"\"Is that glock?\" - Martin Brundle, 2008 Chinese GP",
		"\"Slow button on, slow button on\" - Ferrari F1 Team race engineer",
		"\"HUDSON!\" - the Shambles Championship crew, but mostly Kacey",
		"\"PAPERCUT?!\" - Raycevick, Mexico Shambles GP of 2020",
		"\"The Dollar\" - The Dollar",
		"\"Bababooey\" - Unknown"
	];
	protected static unsafeMisfortunes: string[] = [
		"\"Deez Nuts\" - Juhana The Goblin",
		"\"Suck my balls, mate\" - Kevin Magnussen, 2017 Hungarian GP"
	];

	public static getMisfortune(nsfw?: boolean) {
		nsfw = Boolean(nsfw);

		let target = this.safeMisfortunes;
		if (nsfw) {
			target = this.safeMisfortunes.slice().concat(this.unsafeMisfortunes);
		}

		let r = Math.max(Math.min(Math.random() * target.length, target.length - 1), 0);
		let m = target[Math.floor(r)];
		console.log("Misfortune: ", m, r);
		return m;
	}
}

const CommandsVersion = "0.1.0";
const CommandsVersionString = "jstefanelli.com Command Parser v" + CommandsVersion;
class Commands {
	protected console: MyConsole;
	public constructor(console: MyConsole) {
		this.console = console;
	}

	public clear() {
		this.console.clear();
	}
	public echo(...args: any[]) {
		let ele = document.createElement('div');
		if (args) {
			args.forEach((arg) => {
				ele.innerText += arg + " ";
			})
		}
		this.console.appendElement(ele);
	}

	public help() {
		let ele = document.createElement('div');
		ele.classList.add('whitespace-pre');
		ele.innerHTML += CommandsVersionString + "<br/>";
		ele.innerHTML += "<br/>"
		ele.innerHTML += "Available commands:<br/>";
		ele.innerHTML += "\t<b>jstefanelli.com</b> [-a|-m]<br/>";
		ele.innerHTML += "\t\tMain menu and about screen<br/>";
		ele.innerHTML += "\t<b>clear<br/>";
		ele.innerHTML += "\t\tClear the page<br/>";
		ele.innerHTML += "\t<b>misfortune</b> [--allow-nsfw]<br/>";
		ele.innerHTML += "\t\tGet a random quote from a list of phrases that live rent-free in my mind, sadly<br/>";
		this.console.appendElement(ele);
	}

	public jstefanelli_com(mode: string) {
		if (!mode) {
			mode = '--menu';
		}

		let ele = document.createElement('div');
		if (mode == '--about' || mode == '-a') {
			ele.innerHTML += "<h1>Welcome to <b>JStefanelli.com!</b><br>";
			ele.innerHTML += "Hi, I'm Giovanni (John) Stefanelli. I'm a class 1997 programmer from Italy<br/>";
			ele.innerHTML += "<br/>";
			ele.innerHTML += "I graduated in computer science from the High School ITI P.Hensemberger of Monza in 2017<br/>"
			ele.innerHTML += "<br/>";
			ele.innerHTML += "I mainly dabble in:<br/>";
			ele.innerHTML += "<ul>";
			ele.innerHTML += "<li>OpenGL (Desktop Windows/macOS/Linux, mobile OpenGL ES Android)</li>";
			ele.innerHTML += "<li>Generic mobile development (Android Native, iOS/Android with Xamarin)</li>";
			ele.innerHTML += "<li>Generic Desktop development (C#, Java on Windows, Xamarin.macOS on macOS)</li>";
			ele.innerHTML += "<li>Generic web development (PHP 5/7, Laravel, Html, CSS/SASS, Javascript/Typescript, Vue.js)</li>";
			ele.innerHTML += "</ul>";
			ele.innerHTML += "<br/>";
			ele.innerHTML += "Since late 2018 I've been working for ARtGlass US doing reasearch and development in the field of augmented reality, 3D realtime rendering, web development and Desktop/Mobile development<br/>";
			ele.innerHTML += "<br/>";
			ele.innerHTML += "You can contact me at <a href='mailto:jpdszn@gmal.com'>jpsdzn@gmail.com</a>, on <a href='https://twitter.com/_jstefanelli'>Twitter</a> and <a href='https://reddit.com/u/jpsgfx'>Reddit</a><br/>";
			ele.innerHTML += "<br/>";
			ele.innerHTML += "To check out my hobby projects, visit my <a href='https://github.com/jstefanelli'>GitHub</a>";
		} else if (mode == '--menu' || mode == '-m') {
			let aboutLink = document.createElement('a');
			aboutLink.innerText = ">> about";
			aboutLink.className = "block";
			aboutLink.href = "#";
			aboutLink.addEventListener('click', () => {
				ele.remove();
				activeConsole.printCmd("jstefanelli.com --about").then((c) => {
					c.submitCommand();
				});
			});
			ele.appendChild(aboutLink);

			let helpLink = document.createElement('a');
			helpLink.innerText = ">> help";
			helpLink.className = "block";
			helpLink.href = "#";
			helpLink.addEventListener('click', () => {
				ele.remove();
				activeConsole.printCmd('help').then((c) => {
					c.submitCommand();
				})
			})
			ele.appendChild(helpLink);
		} else {
			ele.innerText = "Unknown mode '" + mode + "'";
		}
		this.console.appendElement(ele);
	}

	public misfortune(nsfw: string) {
		let allowNsfw = nsfw == '--allow-nsfw';

		const misfortune = Misfortunes.getMisfortune(allowNsfw);
		const start = misfortune.substring(0, misfortune.lastIndexOf('-'));
		const end = misfortune.substring(misfortune.lastIndexOf('-'));
		let ele = document.createElement('div');
		let italics = document.createElement('i');
		italics.innerText = start;
		const author = document.createTextNode(end);
		ele.appendChild(italics);
		ele.appendChild(author);

		this.console.appendElement(ele);
	}
}

const aliasMap: Map<string, string> = new Map();
aliasMap.set("jstefanelli.com", "jstefanelli_com");

const activeConsole = new MyConsole(document.getElementById('console-container'), {
	onCommand: (cmdLine, console) => {
		const cmds = new Commands(console) as any;
		const command = new CommandParser(cmdLine);
		
		const alias = aliasMap.has(command.Command) ? aliasMap.get(command.Command) : command.Command;

		const action = cmds[alias];
		if (action && typeof(action) == 'function') {
			action.apply(cmds, command.Arguments);
		} else if(alias.length > 0) {
			const errorElement = document.createElement('div');
			errorElement.innerText = "Command '" + command.Command + "' not found.";
			console.appendElement(errorElement);
		}
	}
});
activeConsole.printCmd('jstefanelli.com').then(() => activeConsole.submitCommand());