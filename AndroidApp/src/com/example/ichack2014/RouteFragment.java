package com.example.ichack2014;

import java.io.IOException;
import java.util.List;
import java.util.UUID;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.location.Address;
import android.location.Criteria;
import android.location.Geocoder;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.example.ichack2014.fileio.AndroidFileIO;
import com.example.ichack2014.fileio.FileIO;
import com.getpebble.android.kit.PebbleKit;
import com.getpebble.android.kit.PebbleKit.PebbleAckReceiver;
import com.getpebble.android.kit.PebbleKit.PebbleNackReceiver;
import com.getpebble.android.kit.util.PebbleDictionary;

public class RouteFragment extends Fragment implements LocationListener {
	
	private final static UUID PEBBLE_APP_UUID = UUID
			.fromString("09f1203a-3f08-498d-93a4-4003cba6ce05");

	private TextView latituteField;
	private TextView longitudeField;
	private TextView latituteDestField;
	private TextView longitudeDestField;
	private Button btnGo;
	private EditText txtPostCode;
	private LocationManager locationManager;
	private String provider;
	Location location;

	private Geocoder coder;

	private static String pebbelData[] = new String[16];

	private Locations locations;

	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState) {
		
		View rootView = inflater.inflate(R.layout.fragment_route,
				container, false);
		
        // Get a handle to the Map Fragment
        //GoogleMap map = ((MapFragment) getFragmentManager()
       //         .findFragmentById(R.id.map)).getMap();
		
		// Get the location manager
		locationManager = (LocationManager) getActivity().getSystemService(Context.LOCATION_SERVICE);
		// Define the criteria how to select the locatioin provider -> use
		// default
		Criteria criteria = new Criteria();
		provider = locationManager.getProviders(criteria, true).get(0);
		Log.d("---", provider.toString());
		location = locationManager.getLastKnownLocation(provider);
		
		FileIO fileIO = new AndroidFileIO(getActivity());
		locations = new Locations(fileIO);

		pebbelData = locations.load();
		pebbleInit();

		pebbleSendStringArray();

		coder = new Geocoder(getActivity());

		if (coder == null) {
			Toast.makeText(getActivity(), "Geocoder was null :(",
					Toast.LENGTH_LONG).show();
			return rootView;
		}

		latituteField = (TextView) rootView.findViewById(R.id.TextView02);
		longitudeField = (TextView) rootView.findViewById(R.id.TextView04);
		latituteDestField = (TextView) rootView.findViewById(R.id.lblDestLatitude);
		longitudeDestField = (TextView) rootView.findViewById(R.id.lblDestLongitude);

		// Initialize the location fields
		if (location != null) {
			System.out.println("Provider " + provider + " has been selected.");
			onLocationChanged(location);
		} else {
			latituteField.setText("Location not available");
			longitudeField.setText("Location not available");
		}

		btnGo = (Button) rootView.findViewById(R.id.btnGo);

		txtPostCode = (EditText) rootView.findViewById(R.id.txtPostCode);

		btnGo.setOnClickListener(new View.OnClickListener() {

			@Override
			public void onClick(View v) {

				String postCode = txtPostCode.getText().toString();

				if (postCode.equals("")) {
					Toast.makeText(getActivity(),
							"Enter a post code mush!", Toast.LENGTH_LONG)
							.show();
					return;
				}

				int dist = RouteFragment.this.setupLocation(postCode);
				Log.d("Distance button click", ""+dist);
			}
		});
		
		
		
		return rootView;
	}

	@Override
	public void onLocationChanged(Location location) {
		double lat = location.getLatitude();
		double lng = location.getLongitude();
		latituteField.setText(String.valueOf(lat));
		longitudeField.setText(String.valueOf(lng));
	}

	@Override
	public void onStatusChanged(String provider, int status, Bundle extras) {
		// TODO Auto-generated method stub

	}

	@Override
	public void onProviderEnabled(String provider) {
		Toast.makeText(getActivity(), "Enabled new provider " + provider,
				Toast.LENGTH_SHORT).show();

	}

	@Override
	public void onProviderDisabled(String provider) {
		Toast.makeText(getActivity(), "Disabled provider " + provider,
				Toast.LENGTH_SHORT).show();
	}

	private void pebbleInit() {

		boolean connected = PebbleKit.isWatchConnected(getActivity());
		String message = "Pebble is "
				+ (connected ? "connected" : "not connected");

		Toast.makeText(getActivity(), message, Toast.LENGTH_SHORT).show();

		Log.d("Pebble state", message);

		PebbleKit.registerPebbleConnectedReceiver(getActivity(),
				new BroadcastReceiver() {
					@Override
					public void onReceive(Context context, Intent intent) {
						Log.i("Pebble state", "Pebble connected!");
					}
				});

		PebbleKit.registerPebbleDisconnectedReceiver(getActivity(),
				new BroadcastReceiver() {
					@Override
					public void onReceive(Context context, Intent intent) {
						Log.i("Pebble state", "Pebble disconnected!");
					}
				});
		
		
		PebbleKit.registerReceivedAckHandler(getActivity(),
				new PebbleAckReceiver(PEBBLE_APP_UUID) {
					@Override
					public void receiveAck(Context context, int transactionId) {
						Log.i("",
								"Received ack for transaction " + transactionId);
						
						PebbleDictionary pebbleDic = new PebbleDictionary();
						
						if(i > 15)
							return;
						
							pebbleDic.addString(i%2 + 1, pebbelData[i]);
							PebbleKit.sendDataToPebble(getActivity(),
									PEBBLE_APP_UUID, pebbleDic);
							i++;
							
							
					}
				});

		
		PebbleKit.registerReceivedNackHandler(getActivity(),
				new PebbleNackReceiver(PEBBLE_APP_UUID) {
					@Override
					public void receiveNack(Context context, int transactionId) {
						Log.i("",
								"Received nack for transaction "
										+ transactionId);
					}
				});

		PebbleKit.registerReceivedDataHandler(getActivity(),
				new PebbleKit.PebbleDataReceiver(PEBBLE_APP_UUID) {
					@Override
					public void receiveData(final Context context,
							final int transactionId, final PebbleDictionary data) {

						RouteFragment.this.processDataFromPebble(data);
						PebbleKit.sendAckToPebble(getActivity(),
								transactionId);
					}
				});
	}

	public int i = 1;
	
	protected void processDataFromPebble(PebbleDictionary data) {
		
		Log.i("DATA FROM PEBBLE", "Received value=" + data.getInteger(1)
				+ " for key: 1");

		
		if(data.getInteger(1) == null){
			return;
		}
		
		int index = (int) (data.getInteger(1) * 2) + 1;

		if(index < 0 || index >= 16){
			Log.d("DATA FROM PEBBLE", "Index from pebble is out of bounds");
			return;
		}
		
		String address = pebbelData[index];
		int distance = setupLocation(address);
		pebbleSendInt(distance);
	}

	private void pebbleSendStringArray() {

		PebbleDictionary pebbleDic = new PebbleDictionary();

		pebbleDic.addString(1, pebbelData[0]);
		PebbleKit.sendDataToPebble(getActivity(),
				PEBBLE_APP_UUID, pebbleDic);
		
		i = 1;
/*		
		for (int i = 0; i < pebbelData.length; i++) {
			pebbleDic.addString(1, pebbelData[i]);
			PebbleKit.sendDataToPebble(getActivity(),
					PEBBLE_APP_UUID, pebbleDic);
			AckID++;
		}

		PebbleKit.sendDataToPebbleWithTransactionId(getActivity(),
				PEBBLE_APP_UUID, pebbleDic, AckID);
		AckID++;
		*/
	}
	
	private void pebbleSendInt(int value) {

		PebbleDictionary pebbleDic = new PebbleDictionary();

		pebbleDic.addInt32(0, value);
		
		PebbleKit.sendDataToPebble(getActivity(),
				PEBBLE_APP_UUID, pebbleDic);

	}

	private int setupLocation(String addressText) {

		try {
			List<Address> addresses = coder.getFromLocationName(addressText, 1);

			if (addresses == null) {
				Toast.makeText(getActivity(),
						"Could not find location from postcode :(",
						Toast.LENGTH_LONG).show();
				return -1;
			} else if (addresses.size() == 0) {
				Toast.makeText(getActivity(),
						"No locations matching that postcode :(",
						Toast.LENGTH_LONG).show();
				return -1;
			}

			Address address = addresses.get(0);

			double lat = address.getLatitude();
			double lon = address.getLongitude();
			
			latituteDestField.setText("" + lat);
			longitudeDestField.setText("" + lon);

			Intent pebbleIntent = new Intent(getActivity(),
					PebbleAlert.class);
			PendingIntent pendingIntent = PendingIntent.getService(
					getActivity(), 0, pebbleIntent, 0);

			locationManager.addProximityAlert(lat, lon, 200, -1, pendingIntent);
			
			Toast.makeText(getActivity(),
					"Done! Pebble will wake you near your destination!",
					Toast.LENGTH_LONG).show();

			Location toLocation = new Location("MyPosition");
			
			toLocation.setLatitude(lat);
			toLocation.setLongitude(lon);
			
			float dist = location.distanceTo(toLocation);
			
			Log.d("DISTANCE",""+dist/1000);
			
			return (int)(dist/1000);
			
		} catch (IOException e) {
			Toast.makeText(
					getActivity(),
					"Something went wrong while getting the location of your destination :(",
					Toast.LENGTH_LONG).show();
			e.printStackTrace();
		}
		return -1;
	}
	
}
