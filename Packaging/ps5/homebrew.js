
async function main() {
    const PAYLOAD = window.workingDir + '/devilutionx.elf';

    return {
        mainText: "DevilutionX",
        secondaryText: 'Diablo build for modern OSes',
        onclick: async () => {
	    return {
		path: PAYLOAD,
		args: ''
	    };
        },
	options: [
	    {
		text: "Force Shareware mode",
		onclick: async () => {
		    return {
			path: PAYLOAD,
			args: '--spawn'
		    };
		}
            },
	    {
		text: "Force Diablo mode",
		onclick: async () => {
		    return {
			path: PAYLOAD,
			args: '--diablo'
		    };
		}
	    },
	    {
		text: "Force Hellfire mode",
		onclick: async () => {
		    return {
			path: PAYLOAD,
			args: '--hellfire'
		    };
		}
	    }
	]
    };
}
