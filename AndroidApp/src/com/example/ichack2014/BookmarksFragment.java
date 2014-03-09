package com.example.ichack2014;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ListView;

public class BookmarksFragment extends Fragment{

	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState) {
		
		View rootView = inflater.inflate(R.layout.fragment_bookmarks,
				container, false);
		
		ListView list = (ListView) rootView.findViewById(R.id.lstBookmaks);
		String[] array = new String[]{"Item Bookmark 1", "Item Bookmark 2", "Item Bookmark 3"};
		
		ArrayAdapter<String> adapter = new ArrayAdapter<String>(getActivity(), R.layout.list_item, array);
		list.setAdapter(adapter);
		
		return rootView;
	}
	
}
