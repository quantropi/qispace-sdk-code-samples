package com.example.demoqispaceapp

import android.app.Activity
import android.content.Intent
import android.os.Build
import android.os.Bundle
import android.widget.Button
import android.widget.TextView
import androidx.annotation.RequiresApi
import com.android.volley.Request
import com.android.volley.RequestQueue
import com.android.volley.Response
import com.android.volley.toolbox.JsonObjectRequest
import com.android.volley.toolbox.Volley
import org.json.JSONObject
import java.util.*
import com.example.demoqispaceapp.MainActivity

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

class sequrActivity : Activity() {
    private lateinit var QE_rq: RequestQueue

    // QiSpace API Call attributes
    private val QE_URL = QiSpace_Info.QISPACE_ENTERPRISE_BASE_URL+"/api/v1/qe/50"
    private val SUBKEY_URL = QiSpace_Info.QISPACE_ENTERPRISE_BASE_URL+"/api/v1/sub_key"

    // Decoding-related attributes
    private lateinit var encoded_QE: ByteArray
    private lateinit var QE: ByteArray
    private var random_str : ByteArray = ByteArray(32)      // Note: For this demo and its purposes, random string is set to be 32 bytes

    private val sequr_wrapper = sequrKotlinWrapper()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.sequr_layout)

        QE_rq = Volley.newRequestQueue(this)

        // Initialize UI elements
        val subkey_status = findViewById<TextView>(R.id.subkey_status)
        val decoded_QE = findViewById<TextView>(R.id.decoded_QE)
        var random_number_display = findViewById<TextView>(R.id.text_rand_display)

        // Random string generator attribute
        var pQrnd_handle : Long = 0

        // Initialize HTTP request object for QiSpace REST API to obtain subkey
        /**
         * Sends a POST request to obtain
         * the subkey, a necessary component to decrypting the payload,
         * in hex string format.
         */
        val subkey_request = object:
            JsonObjectRequest(Request.Method.POST, SUBKEY_URL, null, Response.Listener<JSONObject>{ response ->

                // convert sub_key from hex string to byte array
                MainActivity.sub_key = response["sub_key"].toString().decodeHex()
                subkey_status.text = "SUBKEY OBTAINED"
            }, Response.ErrorListener { error ->
                println("Error: %s".format(error.toString()))
            }){

            // Provide mandatory HTTP headers to obtain payload
            override fun getHeaders(): MutableMap<String, String> {
                val headers = HashMap<String, String>()
                headers["Authorization"] = MainActivity.token
                headers["Content-Type"] = "application/json"
                return headers
            }
        }

        // QiSpace API Call to get payload
        /**
         * QiSpace API response to a GET request consists of
         * a payload, a pre-encrypted Base64 Quantum Entropy (QE) value,
         * and an iv also in Base64, which diminishes data encryption repetition
         */
        val QE_request : JsonObjectRequest = @RequiresApi(Build.VERSION_CODES.O)
        object: JsonObjectRequest(Request.Method.GET, QE_URL, null, Response.Listener{ response ->
            lateinit var qe_iv:ByteArray

            // Converts from Base64 to byte array format
            encoded_QE = Base64.getDecoder().decode(
                response["payload"].toString().toByteArray(Charsets.UTF_8)
            )
            qe_iv = Base64.getDecoder().decode(
                response["iv"].toString().toByteArray(Charsets.UTF_8)
            )

            // Performs SEQUR decoding on QE
            QE = ByteArray(encoded_QE.size)       //Note: decrypted byte array message must be same length as encrypted byte array for decoding to work
            val ret = sequr_wrapper.decode(MainActivity.sub_key, qe_iv, encoded_QE, QE)

            // Formats the QE byte array to hex string string for output
            decoded_QE.text = byteArrayToString(QE)

            // Reset pQrnd handle
            pQrnd_handle = 0
        }, Response.ErrorListener{ error ->
            println("Error: %s".format(error.toString()))
        }){

            // Providing additional login credentials necesary to obtain a response fro the QiSpace API
            override fun getHeaders(): MutableMap<String, String>{
                val headers = HashMap<String, String>()
                headers["Authorization"] = MainActivity.token
                headers["Content-Type"] = "application/json"
                return headers
            }
        }

        // BUTTON: Demonstrate sub_key setup (normally, sub_key generated on device setup)
        findViewById<Button>(R.id.button_setup_subkey).setOnClickListener{
            //Trigger QiSpace API call to get subkey
            QE_rq.add(subkey_request)
        }

        // BUTTON: Handles QiSpace API to get QE (Quantum Entropy)
        findViewById<Button>(R.id.button_get_qe).setOnClickListener{
            // Check if sub_key is null
            if(MainActivity.sub_key == null)
                decoded_QE.text = "Need to initialize subkey before getting QE"
            else
                // Trigger QiSpace API call to get QE (Quantum Entropy)
                QE_rq.add(QE_request)
        }

        // BUTTON: Handles random number generation using pQrnd
        findViewById<Button>(R.id.button_get_rand).setOnClickListener{
            if(!(this::QE.isInitialized)){
                random_number_display.text = "Need to get QE before generating random string"
            }
            else{
                // Seeds pQrnd for the new QE to generate a random value
                if(pQrnd_handle == 0.toLong())   pQrnd_handle = sequr_wrapper.pQrnd_setup(QE);

                // Generate random number per button press
                var ret : Int = sequr_wrapper.pQrnd_generate(pQrnd_handle, random_str)
                random_number_display.text = byteArrayToString(random_str)

                // pQrnd will be closed once you leave this page
            }
        }

        // BUTTON: back to landing page
        findViewById<Button>(R.id.button_sequr_back).setOnClickListener{
            val sequrToLanding = Intent(this, MainActivity::class.java)

            // Cleanup pqrnd handle (continued from button)
            if(pQrnd_handle != 0.toLong()) sequr_wrapper.pQrnd_close(pQrnd_handle)

            startActivity(sequrToLanding)
        }
    }

    // Helper method for converting hex string to byte array
    fun String.decodeHex(): ByteArray {
        check(length % 2 == 0) { "Must have an even length" }
        return chunked(2)
            .map { it.toInt(16).toByte() }
            .toByteArray()
    }

    // Helper method for convenient formatting
    fun byteArrayToString(input: ByteArray) : String{
        var decode_string : String = ""
        for(b in 0..input.size-1){
            decode_string += String.format("%x", input[b])
        }
        return decode_string
    }
}