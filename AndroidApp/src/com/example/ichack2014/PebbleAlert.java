package com.example.ichack2014;

import android.app.Activity;
import android.os.Bundle;
import android.widget.Toast;

public class PebbleAlert extends Activity{

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		Toast.makeText(this, "WAKE UP", Toast.LENGTH_LONG).show();
	}
}
