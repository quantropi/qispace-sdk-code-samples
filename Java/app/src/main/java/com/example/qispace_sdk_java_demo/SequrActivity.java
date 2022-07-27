package com.example.qispace_sdk_java_demo;

import android.app.Activity;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import androidx.annotation.Nullable;
import androidx.annotation.RequiresApi;

import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.JsonObjectRequest;
import com.android.volley.toolbox.Volley;

import org.json.JSONException;
import org.json.JSONObject;

import java.nio.charset.StandardCharsets;
import java.util.Base64;
import java.util.HashMap;

/** To decode any arbitrary payload from QiSpace RESTful API, we use SEQUR decode().
 * This demo SEQUR decoding applied to QE (Quantum Entropy)
 * Prerequisite: A device token for QiSpace API call
 *
 * Note: Only one subkey and one active token per enterprise device
 *
 * Logical flow:
 * 1. Press 'Setup subkey' to establish the sub_key between client and QiSpace Enterprise
 * 2. Press 'Get QE' to get (and decode) the Quantum Entropy (QE) from QISpace Enterprise
 * 3. Press 'Get PQRND' to get the pQrnd string (by seeding it with the retrieved QE)
 */
public class SequrActivity extends Activity {

    private sequrJavaWrapper sequr_wrapper = new sequrJavaWrapper();

    // QiSpace API Call attributes
    private String SUBKEY_URL = QiSpace_Info.QISPACE_ENTERPRISE_BASE_URL+"/api/v1/sub_key";
    private String QE_URL = QiSpace_Info.QISPACE_ENTERPRISE_BASE_URL+"/api/v1/qe/50";       // Note: For this demo, the amount of bytes for Quantum Entropy is specified to be 50

    // Decoding-related attributes
    private byte[] encoded_QE;
    private byte[] QE;
    private byte[] random_str = new byte[32];       // Note: the Pseudo Quantum Random Data is set to be 32 bytes

    private long pQrnd_handle;

    private RequestQueue QE_rq;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.sequr_layout);

        // Initialize UI elements
        TextView subkey_status = findViewById(R.id.subkey_status);
        TextView decoded_QE = findViewById(R.id.decoded_QE);
        TextView random_number_display = findViewById(R.id.text_rand_display);

        QE_rq = Volley.newRequestQueue(this);

        pQrnd_handle = 0;
        QE = null;

        // Initialize HTTP object for QiSpace REST API to obtain subkey
        /**
         *  Sends a POST request to obtain
         * the subkey, a necessary component to decrypting the payload,
         * in hex string format.
         */
        JsonObjectRequest subkey_request = new JsonObjectRequest
                (Request.Method.POST, SUBKEY_URL, null, new Response.Listener<JSONObject>(){
                    @Override
                    public void onResponse(JSONObject keydata){
                        try {
                            // Convert subkey from hex string format to operable byte array
                            MainActivity.sub_key = hexStringToByteArray(
                                    keydata.get("sub_key").toString()
                            );
                            subkey_status.setText("SUBKEY OBTAINED");

//                            System.out.println("LENGTH IS: "+MainActivity.sub_key.length);
                        } catch (JSONException e) {
                            e.printStackTrace();
                        }
                    }
                }, new Response.ErrorListener(){
                    @Override
                    public void onErrorResponse(VolleyError error){
                        System.err.println("ERROR: "+error.toString());
                    }
                }){
            // Provide mandatory headers to obtain payload
            public HashMap<String, String> getHeaders(){
                HashMap<String, String> headers = new HashMap<String, String>();
                headers.put("Content-Type", "application/json");
                headers.put("Authorization", MainActivity.token);
                return headers;
            }
        };

        // QiSpace API Call to get payload
        /**
         * QiSpace API response to a GET request consists of
         * a payload, a pre-encrypted Base64 Quantum Entropy (QE) value,
         * and an iv also in Base64, which diminishes data encryption repetition
         */
        JsonObjectRequest QE_request = new JsonObjectRequest
                (Request.Method.GET, QE_URL, null, new Response.Listener<JSONObject>() {
                    @RequiresApi(api = Build.VERSION_CODES.O)
                    @Override
                    public void onResponse(JSONObject r) {
                        byte[] qe_iv;
                        try {
                            // Converts payload from Base64 to byte array format
//                            System.out.println("STARTING: "+ r.get("payload").toString().getBytes(StandardCharsets.UTF_8));
                            encoded_QE = Base64.getDecoder().decode(
                                    r.get("payload").toString().getBytes(StandardCharsets.UTF_8)
                            );
//                            System.out.println("-------------------------------------------");
                            printArr(encoded_QE);

                            qe_iv = Base64.getDecoder().decode(
                                    r.get("iv").toString().getBytes(StandardCharsets.UTF_8)
                            );

                            // Performs SEQUR decoding on QE
                            QE = new byte[encoded_QE.length];     // Note: decrypted byte array message must be same length as encrypted byte array for decoding to work
                            int ret2 = sequr_wrapper.decode(MainActivity.sub_key, qe_iv, encoded_QE, QE);
                        } catch (JSONException e) {
                            e.printStackTrace();
                        }

                        // Formats QE as hex string for output
                        String decode_string = "";
                        for(int b = 0;b<QE.length;b++){
                            String st = String.format("%x", QE[b]);
                            decode_string +=st;
                        }
                        decoded_QE.setText(decode_string);

                        // Reset pQrnd handle to seed PQRND with new QE
                        pQrnd_handle = 0;
                    }
                }, new Response.ErrorListener() {
                    @Override
                    public void onErrorResponse(VolleyError error) {
                        System.err.println("ERROR: "+error.toString());
                    }
                }){

            // Sets necessary HTTP headers to obtain a response from the QiSpace API
            @Override
            public HashMap<String, String> getHeaders(){
                HashMap<String, String> headers = new HashMap<String, String>();
                headers.put("Content-Type", "application/json");
                headers.put("Authorization", MainActivity.token);
                return headers;
            }
        };

        // Button: Demonstrates subkey setting up (normally, subkey generated on device setup)
        findViewById(R.id.button_setup_subkey).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                // Trigger QiSpace API call to get subkey,
                QE_rq.add(subkey_request);
            }
        });

        // Button: Handles QiSpace API to get QE (Quantum Entropy)
        findViewById(R.id.button_get_qe).setOnClickListener(new View.OnClickListener() {
            @RequiresApi(api = Build.VERSION_CODES.O)
            @Override
            public void onClick(View view) {
                // Check if subkey is null
                if(MainActivity.sub_key == null){
                    decoded_QE.setText("Need to initialize subkey before getting QE");
                    return;
                }
                // Trigger QiSpace API call to get QE (Quantum Entropy)
                QE_rq.add(QE_request);
            }
        });

        // BUTTON: Handles random number generation using pQrnd
        findViewById(R.id.button_get_rand).setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View view){
                if(QE == null){
                    random_number_display.setText("Need to get QE before generating random string");
                    return;
                }

                // Seeds pQrnd with the new QE to generate a random value
                if(pQrnd_handle == 0)   pQrnd_handle = sequr_wrapper.pQrnd_setup(QE);

                // Generate random number per button press
                int ret = sequr_wrapper.pQrnd_generate(pQrnd_handle, random_str);
                random_number_display.setText(byteArrayToString(random_str));

                // pQrnd will be closed after leaving this page
            }
        });

        // BUTTON: Back to landing page
        findViewById(R.id.button_sequr_back).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent sequrToLanding = new Intent(SequrActivity.this, MainActivity.class);

                // Cleanup pqrnd handle (continued from button)
                if(pQrnd_handle != 0)  sequr_wrapper.pQrnd_close(pQrnd_handle);

                startActivity(sequrToLanding);
            }
        });
    }

    // Helper method for converting hex string to byte array
    private static byte[] hexStringToByteArray(String s) {
        int len = s.length();
        byte[] data = new byte[len / 2];
        for (int i = 0; i < len; i += 2) {
            data[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4)
                    + Character.digit(s.charAt(i+1), 16));
        }
        return data;
    }

    // Helper method for convenient formatting
    private static String byteArrayToString(byte[] in){
        String decode_string = "";
        for(int b = 0;b<in.length;b++)
            decode_string += String.format("%x", in[b]);
        return decode_string;
    }

    // Debugging procedure to print contents of byte array
    private static void printArr(byte[] arr){
        for(int i = 0;i<arr.length;i++){
            System.out.print(arr[i]+" ");
        }
        System.out.println();
    }
}
