package org.diasurgical.devilutionx;

import android.Manifest;
import android.annotation.SuppressLint;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;

import org.libsdl.app.SDLActivity;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class DevilutionXSDLActivity extends SDLActivity {
	private String externalDir;

	protected void onCreate(Bundle savedInstanceState) {
		externalDir = getExternalFilesDir(null).getAbsolutePath();

		migrateAppData();

		super.onCreate(savedInstanceState);
	}

	/**
	 * On app launch make sure the game data is present
	 */
	protected void onStart() {
		super.onStart();

		if (missingGameData()) {
			Intent intent = new Intent(this, DataActivity.class);
			startActivity(intent);
			this.finish();
		}
	}

	private boolean missingGameData() {
		File fileLower = new File(externalDir + "/diabdat.mpq");
		File fileUpper = new File(externalDir + "/DIABDAT.MPQ");
		File spawnFile = new File(externalDir + "/spawn.mpq");

		return !fileUpper.exists() && !fileLower.exists() && !spawnFile.exists();
	}

	private boolean copyFile(File src, File dst) {
		try {
			InputStream in = new FileInputStream(src);
			try {
				OutputStream out = new FileOutputStream(dst);
				try {
					// Transfer bytes from in to out
					byte[] buf = new byte[1024];
					int len;
					while ((len = in.read(buf)) > 0) {
						out.write(buf, 0, len);
					}
				} finally {
					out.close();
				}
			} finally {
				in.close();
			}
		} catch (IOException exception) {
			Log.e("copyFile", exception.getMessage());
			if (dst.exists()) {
				//noinspection ResultOfMethodCallIgnored
				dst.delete();
			}
			return false;
		}

		return  true;
	}

	private void migrateFile(File file) {
		if (!file.exists() || !file.canRead()) {
			return;
		}
		File newPath = new File(externalDir + "/" + file.getName());
		if (newPath.exists()) {
			if (file.canWrite()) {
				//noinspection ResultOfMethodCallIgnored
				file.delete();
			}
			return;
		}
		if (!new File(newPath.getParent()).canWrite()) {
			return;
		}
		if (!file.renameTo(newPath)) {
			if (copyFile(file, newPath) && file.canWrite()) {
				//noinspection ResultOfMethodCallIgnored
				file.delete();
			}
		}
	}

	/**
	 * This can be removed Nov 2021 and Google will no longer permit access to the old folder from that point on
	 */
	@SuppressWarnings("deprecation")
	@SuppressLint("SdCardPath")
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
