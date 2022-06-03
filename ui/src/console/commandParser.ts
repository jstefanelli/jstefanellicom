export default class CommandParser {
    protected command: string = '';
    protected arguments: string[] = [];

    public constructor(commandLine: string) {
        commandLine = commandLine.trim();

        let commandFound = false;
        let currentArgument = '';
        let escapeNext = false;
        let inQuotes = false;
        for(let i = 0; i < commandLine.length; i++) {
            let ch = commandLine.charAt(i);
            if (!escapeNext && ch == '\\') {
                escapeNext = true;
                continue;
            }

            if (!escapeNext && ch == '"') {
                inQuotes = !inQuotes;
                continue;
            }

            if (!inQuotes && !escapeNext && (ch == ' ' || ch == '\t')) {
                if (!commandFound) {
                    commandFound = true;
                } else {
                    if (currentArgument.length > 0) {
                        this.arguments.push(currentArgument);
                        currentArgument = '';
                    }
                }
                continue;
            }

            if (!commandFound) {
                this.command += ch;
            } else {
                currentArgument += ch;
            }

            if (escapeNext) {
                escapeNext = false;
            }
        }

        if (currentArgument.length > 0) {
            this.arguments.push(currentArgument);
        }
    }

    public get Command(): string {
        return this.command;
    }

    public get Arguments(): string[] {
        return this.arguments;
    }
}