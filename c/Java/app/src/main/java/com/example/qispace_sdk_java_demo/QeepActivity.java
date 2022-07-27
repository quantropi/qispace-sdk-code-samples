package com.example.qispace_sdk_java_demo;

import androidx.annotation.Nullable;
import androidx.annotation.RequiresApi;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.TextView;

import com.android.volley.AuthFailureError;
import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.JsonObjectRequest;
import com.android.volley.toolbox.Volley;

import org.json.JSONException;
import org.json.JSONObject;

import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.Base64;
import java.util.HashMap;
import java.util.Map;

/**
 * This page demonstrates a use of the QEEP API to encrypt and decrypt a message with a QiSpace retrieved Quantum Key (QK) and iv
 * Additionally, it shows the usage of the SEQUR API as a by-product of unpacking the QK and its IV
 *
 * Prerequisites:
 * - An enterprise device token which will be used for QK retrieval from QiSpace Enterprise REST API
 * - A subkey intended for SEQUR decoding the QK after its retrieval. Realistically, subkey would be initialized on device setup
 *
 * **Ideally, setting up the subkey from SequrActivity must be done before getting QK as QK decryption requires subkey to be initialized
 *
 * Logical Flow:
 * 1. Press 'Get QK' to create and unpack a Quantum Key from QiSpace Enterprise
 * 2. Enter a message into the text field
 * 3. Press 'Encrypt' to QEEP encrypt the text field message
 * 4. Press 'Decrypt' to QEEP decrypt the encrypted text field message
 */

public class QeepActivity extends Activity {

    // Note: Assume sub_key is given for this demo page. Realistically, it is initialized on device setup

    // Note: Currently, max message length is 64;
    private byte[] encrypted_message;
    private byte[] decrypted_message;

    private int messageLen = 0;

    private byte[] qk;
    private byte[] message_iv;      //Note: Must be 16 bytes

    // Initialize the wrapper classes used for decoding and encrypting/decrypting
    qeepJavaWrapper qeep_wrapper = new qeepJavaWrapper();
    sequrJavaWrapper sequr_wrapper = new sequrJavaWrapper();

    private String QK_URL = QiSpace_Info.QISPACE_ENTERPRISE_BASE_URL+"/api/v1/qk";

    private RequestQueue QK_rq;

    private boolean have_successfully_encrypted = false;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.qeep_layout);

        // Initialize UI elements
        EditText message_in = findViewById(R.id.message_in);
        TextView encrypt_result = findViewById(R.id.encrypt_message_result);
        TextView decrypt_result = findViewById(R.id.decrypted_message_result);

        QK_rq = Volley.newRequestQueue(this);

        /**
         * QiSpace REST API response consists of
         * a payload, a pre-encrypted qeep key (QK),
         * and an iv, a variable that diminishes data encryption repetition,
         * all in Base64 format
         */
        // Sets up HTTP body info
        HashMap<String, Object> params = new HashMap<String, Object>();
        params.put("key_length", Integer.valueOf(32));      // For this demo, key_length was set to 32
        params.put("label", "message dev1");

        // Define request object for QiSpace API Call to get the encrypted QK payload
        JsonObjectRequest qk_request = new JsonObjectRequest
                (Request.Method.POST, QK_URL, new JSONObject(params), new Response.Listener<JSONObject>(){
                    @RequiresApi(api = Build.VERSION_CODES.O)
                    @Override
                    public void onResponse(JSONObject response){
                        byte[] qk_iv;
                        byte[] encoded_QK;
                        try {
                            // Gets and converts the payload and iv from Base64 to byte array
                            encoded_QK = Base64.getDecoder().decode(
                                    response.get("payload").toString().getBytes(StandardCharsets.UTF_8)
                            );
                            qk_iv = Base64.getDecoder().decode(
                                    response.get("iv").toString().getBytes(StandardCharsets.UTF_8)
                            );

                            // SEQUR decodes payload to get actual QK
                            qk = new byte[encoded_QK.length];
                            int ret = sequr_wrapper.decode(MainActivity.sub_key, qk_iv, encoded_QK, qk);
                        } catch (JSONException e) {
                            e.printStackTrace();
                        }
                    }
                }, new Response.ErrorListener(){
                    @Override
                    public void onErrorResponse(VolleyError error){
                        System.out.println("ERROR: "+error.toString());
                    }
                }){
            // Sets additional credentials to get payload
            @Override
            public Map<String, String> getHeaders() throws AuthFailureError{
                HashMap<String, String> headers = new HashMap<String, String>();
                headers.put("Authorization", MainActivity.token);
                headers.put("Content-Type", "application/json");
                return headers;
            }
        };

        // BUTTON: Handles QK retrieval using Quantropi's QiSpace API
        findViewById(R.id.button_get_qk).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                // Trigger QiSpace API call to get QK
                QK_rq.add(qk_request);
            }
        });

        // BUTTON: Handles the process of encrypting the message using QK(Qeep Key) and iv
        findViewById(R.id.button_encrypt).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                closeKeyboard();

                // Gets message from text field input
                byte[] message = message_in.getText().toString().getBytes(Charset.forName("UTF-8"));
                messageLen = message.length;
                encrypted_message = new byte[messageLen];
                decrypted_message = new byte[messageLen];

                // Generates fixed fixed IV for this demo's encryption/decryption purposes
                // Note: The IV must be 16 bytes and should be randomly generated per encrypt operation or per message
                message_iv = hexStringToByteArray("0102030405060708090a010203040506");

                // Check qk/message_iv initialized
                if(qk == null){
                    encrypt_result.setText("Need to retrieve qk before encrypting");
                    return;
                }

                // Encrypts message with QEEP wrapper class
                int ret = qeep_wrapper.encrypt(qk, message_iv, message, encrypted_message);

                // Handle UI display with error codes
                if(ret == 1){
                    encrypt_result.setText(byteArrayToString(encrypted_message));
                    have_successfully_encrypted = true;
                }
                else{
                    encrypt_result.setText("Error code "+ret+" occurred in encryption. Consult error.h for more information");
                }
            }
        });

        // BUTTON: Handles the decryption of the message using qk (qeep key) and iv
        findViewById(R.id.button_decrypt).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                closeKeyboard();
                // Check qk initialized
                if(qk == null){
                    decrypt_result.setText("Need to retrieve qk before decrypting");
                    return;
                }
                // Ensures encryption before decryption
                if(!have_successfully_encrypted){
                    decrypt_result.setText("Need to encrypt before decrypting");
                    return;
                }

                // Decrypts the encrypted message
                // Note: Message iv should be identical to one used during encryption
                int ret = qeep_wrapper.decrypt(qk, message_iv, encrypted_message, decrypted_message);

                // Handle UI display with error codes based on decrypt result
                if(ret == 1){
                    decrypt_result.setText(new String(decrypted_message, StandardCharsets.US_ASCII));
                }
                else{
                    decrypt_result.setText("Error code "+ret+" occurred in decryption. Consult error.h for more information");
                }
            }
        });

        // BUTTON: go back to landing page
        findViewById(R.id.button_qeep_back).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent qeepToLanding = new Intent(QeepActivity.this, MainActivity.class);
                startActivity(qeepToLanding);
            }
        });
    }

    // Private Debugging procedure to print contents of byte array
    private static void printArr(byte[] arr){
        for(int i = 0;i<arr.length;i++){
            System.out.print(arr[i]+" ");
        }
        System.out.println();
    }

    private void closeKeyboard(){
        View view = this.getCurrentFocus();
        if(view != null){
            InputMethodManager imm = (InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE);
            imm.hideSoftInputFromWindow(view.getWindowToken(), 0);
        }
    }

    // Helper function to convert hexString to byte array
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
}