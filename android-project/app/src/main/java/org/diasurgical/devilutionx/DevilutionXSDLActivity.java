package org.diasurgical.devilutionx;

import android.Manifest;
import android.app.AlertDialog;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.preference.PreferenceManager;
import android.util.Log;

import org.libsdl.app.SDLActivity;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class DevilutionXSDLActivity extends SDLActivity {
	private final String SharewareTitle = "Shareware version";
	private SharedPreferences mPrefs;
	final private String shouldShowInstallationGuidePref = "shouldShowInstallationGuide";
	private String externalDir;

	protected void onCreate(Bundle savedInstanceState) {
		externalDir = getExternalFilesDir(null).getAbsolutePath();

		migrateAppData();

		if (shouldShowInstallationGuide()) {
			showInstallationGuide();
		}

		super.onCreate(savedInstanceState);
	}

	/**
	 * Check if this is the first time the app is run and the full game data is not already present
	 */
	private boolean shouldShowInstallationGuide() {
		mPrefs = PreferenceManager.getDefaultSharedPreferences(this);
		return mPrefs.getBoolean(shouldShowInstallationGuidePref, !hasFullGameData());
	}

	private boolean hasFullGameData() {
		File fileLower = new File(externalDir + "/diabdat.mpq");
		File fileUpper = new File(externalDir + "/DIABDAT.MPQ");

		return fileUpper.exists() || fileLower.exists();
	}

	private void installationGuideShown() {
		SharedPreferences.Editor editor = mPrefs.edit();
		editor.putBoolean(shouldShowInstallationGuidePref, false);
		editor.apply();
	}

	private void showInstallationGuide() {
		String message = "To play the full version you must place DIABDAT.MPQ from the original game in Android"
				+ externalDir.split("/Android")[1]
				+ ".\n\nIf you don't have the original game then you can buy Diablo from GOG.com.";
		new AlertDialog.Builder(this)
				.setTitle(SharewareTitle)
				.setPositiveButton("OK", null)
				.setMessage(message)
				.show();
		installationGuideShown();
	}

	private boolean copyFile(File src, File dst) {
		try {
			try (InputStream in = new FileInputStream(src)) {
				try (OutputStream out = new FileOutputStream(dst)) {
					// Transfer bytes from in to out
					byte[] buf = new byte[1024];
					int len;
					while ((len = in.read(buf)) > 0) {
						out.write(buf, 0, len);
					}

					return true;
				}
			}
		} catch (IOException exception) {
			Log.e("copyFile", exception.getMessage());
		}

		return false;
	}

	private void migrateFile(File file) {
		if (!file.exists() || !file.canRead()) {
			return;
		}
		File newPath = new File(externalDir + "/" + file.getName());
		if (newPath.exists()) {
			if (file.canWrite()) {
				file.delete();
			}
			return;
		}
		if (!new File(newPath.getParent()).canWrite()) {
			return;
		}
		if (!file.renameTo(newPath)) {
			if (copyFile(file, newPath) && file.canWrite()) {
				file.delete();
			}
		}
	}

	/**
	 * This can be removed Nov 2021 and Google will no longer permit access to the old folder from that point on
	 */
	private void migrateAppData() {
		if (Build.VERSION.SDK_INT > Build.VERSION_CODES.LOLLIPOP_MR1) {
			if (PackageManager.PERMISSION_GRANTED != checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE)) {
				return;
			}
		}

		migrateFile(new File(Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS) + "/diabdat.mpq"));
		migrateFile(new File(Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS) + "/DIABDAT.MPQ"));

		migrateFile(new File("/sdcard/diabdat.mpq"));
		migrateFile(new File("/sdcard/devilutionx/diabdat.mpq"));
		migrateFile(new File("/sdcard/devilutionx/spawn.mpq"));

		for (File internalFile : getFilesDir().listFiles()) {
			migrateFile(internalFile);
		}
	}

	protected String[] getArguments() {
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
				"SDL2",
				"SDL2_ttf",
				"devilutionx"
		};
	}
}
