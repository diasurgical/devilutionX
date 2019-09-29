package org.diasurgical.devilutionx;

import org.libsdl.app.SDLActivity;

public class DevilutionXSDLActivity extends SDLActivity {
	protected String[] getLibraries() {
		return new String[]{
				"SDL2",
				"SDL2_ttf",
				"devilutionx"
		};
	}
}
