package org.diasurgical.devilutionx;

import android.Manifest;
import android.annotation.SuppressLint;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Rect;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.ViewTreeObserver;

import org.libsdl.app.SDLActivity;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Locale;

public class DevilutionXSDLActivity extends SDLActivity {
	private String externalDir;

	protected void onCreate(Bundle savedInstanceState) {
		// windowSoftInputMode=adjustPan stopped working
		// for fullscreen apps after Android 7.0
		if (Build.VERSION.SDK_INT >= 25)
			trackVisibleSpace();

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
		if (file.getName().equals("spawn.mpq"))
			newPath = new File(externalDir + "/" + file.getName() + "-temp");

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

	/**
	 * This method is called by SDL using JNI.
	 */
	public String getLocale()
	{
		return Locale.getDefault().toString();
	}

	protected String[] getArguments() {
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
				"SDL2",
				"SDL2_ttf",
				"devilutionx"
		};
	}
}
