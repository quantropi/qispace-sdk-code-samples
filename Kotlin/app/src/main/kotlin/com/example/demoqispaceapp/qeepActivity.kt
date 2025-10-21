package com.example.demoqispaceapp

import android.app.Activity
import android.content.Context
import android.content.Intent
import android.os.Build.VERSION_CODES
import android.os.Bundle
import android.text.style.TabStopSpan
import android.view.View
import android.view.inputmethod.InputMethodManager
import android.widget.Button
import android.widget.EditText
import android.widget.TextView
import androidx.annotation.Nullable
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AppCompatActivity
import com.android.volley.Request
import com.android.volley.RequestQueue
import com.android.volley.Response
import com.android.volley.toolbox.JsonObjectRequest
import com.android.volley.toolbox.Volley
import com.example.demoqispaceapp.MainActivity.Companion.decodeHex
import org.json.JSONObject
import java.nio.charset.StandardCharsets
import java.util.*

/**
 * This page demonstrates a use of the QEEP API to encrypt and decrypt a message with a QiSpace retrieved Quantum Key (QK) and iv
 * Additionally, it shows the usage of the SEQUR API as a by-product of unpacking the QK and its IV
 *
 * Prerequisites:
 * - An enterprise device token which will be used for QK retrieval from QiSpace Enterprise REST API
 * - A subkey intended for SEQUR decoding the QK after its retrieval. Realistically, subkey would be initialized on device setup
 *
 * **Ideally, setting up the subkey from sequrActivity must be done before getting QK as QK decryption requires subkey to be initialized
 * **Normally, it is initialized on device setup
 *
 * Logical Flow:
 * 1. Press 'Get QK' to create and unpack a Quantum Key from QiSpace Enterprise
 * 2. Enter a message into the text field
 * 3. Press 'Encrypt' to QEEP encrypt the text field message
 * 4. Press 'Decrypt' to QEEP decrypt the encrypted text field message
 */
class qeepActivity : Activity() {

    private lateinit var QK_rq: RequestQueue

    private val QK_URL = QiSpace_Info.QISPACE_ENTERPRISE_BASE_URL+"/api/v1/qk"

    // Note: Currently, maximum message length is 64
    private lateinit var encrypted_message: ByteArray
    private lateinit var decrypted_message: ByteArray

    private var messageLen = 0

    // Initialize payload variables
    private lateinit var QK : ByteArray
    private lateinit var message_iv : ByteArray     // Note: Must be 16 bytes

    // Initialize the wrapper classes used for decoding and encrypting/decrypting
    private val qeep_wrapper = qeepKotlinWrapper()
    private val sequr_wrapper = sequrKotlinWrapper()

    private var have_successfully_encrypted = false;

    override fun onCreate(@Nullable savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.qeep_layout)

        // Initialize UI elements
        val message_in = findViewById<EditText>(R.id.message_in)
        val encrypt_result = findViewById<TextView>(R.id.encrypt_message_result)
        val decrypt_result = findViewById<TextView>(R.id.decrypted_message_result)

        QK_rq = Volley.newRequestQueue(this)

        /**
         *       QiSpace QK API response consists of
         *       a payload, a pre-encoded Qeep Key (QK),
         *       and an iv, a variable that diminishes data encryption repetition,
         *       all in Base64 format
         */
        // Sets up QiSpace API HTTP call body
        val params = JSONObject()
        params.put("key_length", 32)
        params.put("label", "demo-device")

        // QiSpace API Call to get the encrypted QK payload
        val qk_request : JsonObjectRequest = @RequiresApi(VERSION_CODES.O)
        object: JsonObjectRequest(Request.Method.POST, QK_URL, params, Response.Listener{ stuff ->
//            println(stuff)
            // Gets and converts the payload and iv from Base64 to byte array
            var encoded_QK : ByteArray = Base64.getDecoder().decode(
                stuff["payload"].toString().toByteArray(Charsets.UTF_8)
            )
            var qk_iv : ByteArray = Base64.getDecoder().decode(
                stuff["iv"].toString().toByteArray(Charsets.UTF_8)
            )

            QK = ByteArray(encoded_QK.size)
            // SEQUR decode the payload to get actual QK
            val ret = sequr_wrapper.decode(MainActivity.sub_key, qk_iv, encoded_QK, QK)

        }, Response.ErrorListener{ error ->
            println("Error: %s".format(error.toString()))
        }){

            // Sets HTTP request headers to get payload
            override fun getHeaders(): MutableMap<String, String>{
                val headers = HashMap<String, String>()
                headers["Authorization"] = MainActivity.token
                headers["Content-Type"] = "application/json"
                return headers
            }
        }

        // BUTTON: Handles QK retrieval using Quantropi's QiSpace API
        findViewById<Button>(R.id.button_get_qk).setOnClickListener{
            //Trigger QiSpace API call to get QK
            QK_rq.add(qk_request)
        }

        // BUTTON: Handles the process of encoding the message using QK(qeep key) and iv
        findViewById<Button>(R.id.button_encrypt).setOnClickListener {
            hideKeyboardFrom(this, message_in)

            // Get message from user input
            val message = message_in.text.toString().toByteArray(Charsets.UTF_8)
            messageLen = message.size
            encrypted_message = ByteArray(messageLen)
            decrypted_message = ByteArray(messageLen)

            // Generate random 16 bytes (MANDATORY) iv per encrypt operation or per message
            // The message iv is simply fixed for this demo
            message_iv = "0102030405060708090a010203040506".decodeHex()

            // Check qk/message_iv initialized
            if(!(this::QK.isInitialized)){
                encrypt_result.text = "Need to retrieve qk before decryption"
                return@setOnClickListener
            }

            // Encrypt the message with QEEP wrapper class
            val ret = qeep_wrapper.encrypt(QK, message_iv, message, encrypted_message)

            // Handle UI display based on error codes
            if(ret == 1){
                encrypt_result.text = byteArrayToString(encrypted_message)
                have_successfully_encrypted = true;
            }
            else{
                encrypt_result.text = "Error code "+ret+" occurred in encryption. Consult eror.h for more information";
            }
        }

        // BUTTON: Handles the decryption of the message using QK (qeep key) and iv
        findViewById<Button>(R.id.button_decrypt).setOnClickListener {
            hideKeyboardFrom(this, message_in)

            // Check QK & encrypted_message initialized
            if(!(this::QK.isInitialized)){
                decrypt_result.text = "Need to retrieve qk before decryption"
                return@setOnClickListener
            }
            if(!have_successfully_encrypted){
                decrypt_result.text = "No encrypted message available to decrypt"
                return@setOnClickListener
            }

            // Decrypt the encrypted message
            // Message iv should be identical to one used during encryption
            val ret2 = qeep_wrapper.decrypt(QK, message_iv, encrypted_message, decrypted_message)

            // Handle UI display based on error codes
            if(ret2 == 1){
                decrypt_result.text = String(decrypted_message, StandardCharsets.US_ASCII)
            }
            else{
                decrypt_result.text = "Error code "+ret2+" occurred during decryption. Consult error.h for more information"
            }
        }

        // BUTTON: go back to landing page
        findViewById<Button>(R.id.button_qeep_back).setOnClickListener{
            val qeepToLanding = Intent(this, MainActivity::class.java)
            startActivity(qeepToLanding)
        }
    }

    fun hideKeyboardFrom(context: Context, view: View) {
        val imm: InputMethodManager =
            context.getSystemService(INPUT_METHOD_SERVICE) as InputMethodManager
        imm.hideSoftInputFromWindow(view.windowToken, 0)
    }

    // Helper function to convert hexString to byte array
    fun String.decodeHex(): ByteArray {
        check(length % 2 == 0) { "Must have an even length" }
        return chunked(2)
            .map { it.toInt(16).toByte() }
            .toByteArray()
    }

    // Private debugging procedure to print contents of byte array
    private fun printArr(arr: ByteArray) {
        for (i in arr.indices) {
            print(arr[i].toString() + " ")
        }
        println()
    }

    // Helper method for convenient formatting
    private fun byteArrayToString(input : ByteArray) : String{
        var decode_string : String = ""
        for(b in 0..input.size-1){
            decode_string += String.format("%x", input[b])
        }
        return decode_string
    }
}
