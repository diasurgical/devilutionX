package org.diasurgical.devilutionx;

import android.content.Intent;
import android.graphics.Rect;
import android.os.Build;
import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.ViewTreeObserver;

import org.libsdl.app.SDLActivity;

import java.io.File;
import java.util.Locale;

public class DevilutionXSDLActivity extends SDLActivity {
	private ExternalFilesManager fileManager;
	private boolean noExit;

	protected void onCreate(Bundle savedInstanceState) {
		// windowSoftInputMode=adjustPan stopped working
		// for fullscreen apps after Android 7.0
		if (Build.VERSION.SDK_INT >= 25)
			trackVisibleSpace();

		fileManager = new ExternalFilesManager(this);

		migrateSaveGames();

		super.onCreate(savedInstanceState);
	}

	/**
	 * On app launch make sure the game data is present
	 */
	protected void onStart() {
		super.onStart();

		if (isMissingGameData()) {
			Intent intent = new Intent(this, DataActivity.class);
			startActivity(intent);
			noExit = true;
			this.finish();
		}
	}

	/**
	 * When the user exits the game, use System.exit(0)
	 * to clear memory and prevent errors on restart
	 */
	protected void onDestroy() {
		super.onDestroy();

		if (!noExit) {
			System.exit(0);
		}
	}

	private void trackVisibleSpace() {
		this.getWindow().getDecorView().getViewTreeObserver().addOnGlobalLayoutListener(new ViewTreeObserver.OnGlobalLayoutListener() {
			@Override
			public void onGlobalLayout() {
				// Software keyboard may encroach on the app's visible space so
				// force the drawing surface to fit in the visible display frame
				Rect visibleSpace = new Rect();
				getWindow().getDecorView().getWindowVisibleDisplayFrame(visibleSpace);

				SurfaceView surface = mSurface;
				SurfaceHolder holder = surface.getHolder();
				holder.setFixedSize(visibleSpace.width(), visibleSpace.height());
			}
		});
	}

	private boolean isMissingGameData() {
		String lang = Locale.getDefault().toString();
		if (lang.startsWith("pl") && !fileManager.hasFile("pl.mpq"))
			return true;
		if (lang.startsWith("ru") && !fileManager.hasFile("ru.mpq"))
			return true;
		if (lang.startsWith("ko") || lang.startsWith("zh") || lang.startsWith("ja")) {
			if (!fileManager.hasFile("fonts.mpq") ||
					fileManager.fileSize("fonts.mpq") == 70471463 /* v1 */ ||
					fileManager.fileSize("fonts.mpq") == 53991069 /* v2 */ ||
					fileManager.fileSize("fonts.mpq") == 58488019 /* v3 */) {
				return true;
			}
		}

		return !fileManager.hasFile("diabdat.mpq") &&
				!fileManager.hasFile("DIABDAT.MPQ") &&
				!fileManager.hasFile("spawn.mpq");
	}

	private void migrateSaveGames() {
		File[] files = getFilesDir().listFiles();
		if (files == null)
			return;
		for (File internalFile : files) {
			fileManager.migrateFile(internalFile);
		}
	}

	/**
	 * This method is called by SDL using JNI.
	 */
	public String getLocale()
	{
		return Locale.getDefault().toString();
	}

	protected String[] getArguments() {
		String externalDir = fileManager.getExternalFilesDirectory();

		if (BuildConfig.DEBUG) {
			return new String[]{
				"--data-dir",
				externalDir,
				"--config-dir",
				externalDir,
				"--save-dir",
				externalDir,
				"--verbose",
			};
		}

		return new String[]{
			"--data-dir",
			externalDir,
			"--config-dir",
			externalDir,
			"--save-dir",
			externalDir
		};
	}

	protected String[] getLibraries() {
		return new String[]{
				"devilutionx"
		};
	}
}
