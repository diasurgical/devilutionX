package org.diasurgical.devilutionx;

import android.Manifest;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.preference.PreferenceManager;

import org.libsdl.app.SDLActivity;

import java.io.File;

public class DevilutionXSDLActivity extends SDLActivity {
	final String permission = Manifest.permission.READ_EXTERNAL_STORAGE;
	private Handler permissionHandler;
	private AlertDialog PermissionDialog;
	private final String SharewareTitle = "Shareware version";
	private SharedPreferences mPrefs;
	final private String shouldShowInstallationGuidePref = "shouldShowInstallationGuide";

	protected void onCreate(Bundle savedInstanceState) {
		checkStoragePermission();

		super.onCreate(savedInstanceState);
	}

	private boolean shouldShowInstallationGuide(boolean isDataAccessible) {
		mPrefs = PreferenceManager.getDefaultSharedPreferences(this);
		return mPrefs.getBoolean(shouldShowInstallationGuidePref, !isDataAccessible);
	}

	private void installationGuideShown() {
		SharedPreferences.Editor editor = mPrefs.edit();
		editor.putBoolean(shouldShowInstallationGuidePref, false);
		editor.apply();
	}

	private void checkStoragePermission() {
		if (Build.VERSION.SDK_INT <= Build.VERSION_CODES.LOLLIPOP_MR1) {
			checkStoragePermissionLollipop();
			return;
		}

		if (PackageManager.PERMISSION_GRANTED != checkSelfPermission(permission)) {
			requestPermissions(new String[]{permission}, 1);
		}

		boolean dataFileFound = hasDataFile();
		boolean shouldShowInstallationGuide = shouldShowInstallationGuide(dataFileFound && PackageManager.PERMISSION_GRANTED == checkSelfPermission(permission));

		if (!shouldShowInstallationGuide && !shouldShowRequestPermissionRationale(permission)) {
			return;
		}

		String message = "To play the full version you must grant the application read access to " + System.getenv("EXTERNAL_STORAGE") + ".";
		if (!dataFileFound) {
			message = "To play the full version you must place DIABDAT.MPQ from the original game in " + System.getenv("EXTERNAL_STORAGE") + " and grant the application read access.\n\nIf you don't have the original game then you can buy Diablo from GOG.com.";
			installationGuideShown();
		}

		permissionHandler = new Handler();

		PermissionDialog = new AlertDialog.Builder(this)
				.setTitle(SharewareTitle)
				.setPositiveButton("OK", new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int id) {
						permissionHandler.post(new Runnable() {
							@Override
							public void run() {
								throw new RuntimeException();
							}
						});
					}
				})
				.setMessage(message)
				.show();

		if (dataFileFound) {
			triggerPulse(); // Start full game as soon as we have file access
		}

		try {
			Looper.loop();
		} catch (RuntimeException ignored) {
		}
	}

	private void checkStoragePermissionLollipop() {
		boolean dataFileFound = hasDataFile();
		boolean shouldShowInstallationGuide = shouldShowInstallationGuide(dataFileFound);
		if (!shouldShowInstallationGuide) {
			return;
		}

		String message = "To play the full version you must place DIABDAT.MPQ from the original game in " + System.getenv("EXTERNAL_STORAGE") + ".\n\nIf you don't have the original game then you can buy Diablo from GOG.com.";
		new AlertDialog.Builder(this)
				.setTitle(SharewareTitle)
				.setPositiveButton("OK", null)
				.setMessage(message)
				.show();
		installationGuideShown();
	}

	private void triggerPulse() {
		permissionHandler.postDelayed(new Runnable() {
			@Override
			public void run() {
				if (PackageManager.PERMISSION_GRANTED == checkSelfPermission(permission)) {
					PermissionDialog.hide();
					throw new RuntimeException();
				}
				triggerPulse();
			}
		}, 50);
	}

	private boolean hasDataFile() {
		File fileLower = new File(System.getenv("EXTERNAL_STORAGE") + "/diabdat.mpq");
		File fileUpper = new File(System.getenv("EXTERNAL_STORAGE") + "/DIABDAT.MPQ");

		return fileUpper.exists() || fileLower.exists();
	}

	protected String[] getLibraries() {
		return new String[]{
				"SDL2",
				"SDL2_ttf",
				"devilutionx"
		};
	}
}
