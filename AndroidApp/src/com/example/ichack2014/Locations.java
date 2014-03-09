package com.example.ichack2014;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;

import android.util.Log;

import com.example.ichack2014.fileio.FileIO;

public class Locations {

	public final static String file = ".snooze";
	
	private final static int SIZE = 16;
	private FileIO fileIO;
	
	Locations(FileIO fileIO){
		this.fileIO = fileIO;
	}
	
    public String[] load() {
        BufferedReader in = null;
        
        String[] data = new String[SIZE];
        
        for(int i = 0; i < SIZE; i+=2){
        	data[i] = "Name " + i;
        	data[i+1] = "Address " + i;
        }
        
        try {
            in = new BufferedReader(new InputStreamReader(fileIO.readFile(file)));
            
            for(int i = 0; i < SIZE; i+=2) {
            	data[i] = in.readLine();
            	data[i+1] = in.readLine();
            }
        } catch (IOException e) {
        } catch (NumberFormatException e) {
        } finally {
            try {
                if (in != null)
                    in.close();
            } catch (IOException e) {
            }
        }
        return data;
    }

    public void save(String[] data) {
        BufferedWriter out = null;
        try {
            out = new BufferedWriter(new OutputStreamWriter(
            		fileIO.writeFile(file)));
            
            for(int i = 0; i < SIZE; i+=2) {
                out.write(data[i]);
                out.write("\n");
                out.write(data[i+1]);
                out.write("\n");
            }

        } catch (IOException e) {
        } finally {
            try {
                if (out != null)
                    out.close();
            } catch (IOException e) {
            }
        }
    }
	
}
