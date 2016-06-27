package com.gaulois94;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintWriter;

import android.app.Activity;
import android.content.Context;
import android.util.Log;

public class Tool
{
	public static String copyAssetToData(Context context, String fileName)
	{
		return Tool.copyAssetTo(context, fileName, context.getFilesDir().getAbsolutePath() + "/" + fileName);
	}

	public static String copyAssetTo(Context context, String input, String output)
	{
		Log.e("Engine", "File " + input + " is copying");
		try
		{
			InputStream in       = context.getAssets().open(input);
			FileOutputStream out = new FileOutputStream(output);
			byte[] buffer = new byte[1024];
			int read;
			while((read = in.read(buffer)) != -1)
				out.write(buffer, 0, read);
			in.close();
			out.flush();
			out.close();
		}
		catch (Exception e)
		{
			Log.e("Engine", "Error copying file : " + e.getMessage());
			return null;
		}
		return output;
	}

	public static boolean isFileInData(Context context, String fileName)
	{
		File file = new File(context.getFilesDir().getAbsolutePath() + "/" + fileName);
		return file.exists();
	}
}
