package org.diasurgical.devilutionx;

import android.app.Activity;
import android.content.ClipData;
import android.content.ContentResolver;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.support.annotation.Nullable;
import android.os.Bundle;
import android.support.annotation.RequiresApi;
import android.support.v4.provider.DocumentFile;
import android.util.Log;

import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Objects;

public class ImportActivity extends Activity {

	private static final int IMPORT_REQUEST_CODE = 0xD1AB70;

	@RequiresApi(api = Build.VERSION_CODES.KITKAT)
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
		intent.addCategory(Intent.CATEGORY_OPENABLE);
		intent.putExtra(Intent.EXTRA_ALLOW_MULTIPLE, true);
		intent.setType("*/*");
		startActivityForResult(intent, IMPORT_REQUEST_CODE);
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
		if (requestCode != IMPORT_REQUEST_CODE)
			return;

		if (resultCode == Activity.RESULT_OK && data != null) {
			importFile(data.getData());
			handleClipData(data.getClipData());
		}

		finish();
	}

	private void handleClipData(ClipData clipData) {
		if (clipData == null)
			return;

		for (int i = 0; i < clipData.getItemCount(); i++) {
			ClipData.Item item = clipData.getItemAt(i);
			if (item == null)
				continue;

			importFile(item.getUri());
		}
	}

	private void importFile(Uri fileUri) {
		if (fileUri == null)
			return;

		DocumentFile file = Objects.requireNonNull(DocumentFile.fromSingleUri(getApplicationContext(), fileUri));
		String fileName = file.getName();
		String externalFilesDir = getExternalFilesDir(null).getAbsolutePath();
		String externalFilesPath = externalFilesDir + "/" + fileName;

		try {
			InputStream inputStream = null;
			OutputStream outputStream = null;

			try {
				ContentResolver contentResolver = getContentResolver();
				inputStream = contentResolver.openInputStream(fileUri);
				outputStream = new FileOutputStream(externalFilesPath);

				// Transfer bytes from in to out
				byte[] buf = new byte[4096];
				int len;
				while ((len = inputStream.read(buf)) > 0) {
					outputStream.write(buf, 0, len);
				}
			} finally {
				if (inputStream != null)
					inputStream.close();
				if (outputStream != null)
					outputStream.close();
			}
		} catch (IOException exception) {
			String message = exception.getMessage();
			if (message == null) {
				Log.e("importFile", "IOException", exception);
			} else {
				Log.e("importFile", message);
			}
		}
	}
}
