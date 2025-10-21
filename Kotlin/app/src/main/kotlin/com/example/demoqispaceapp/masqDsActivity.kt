package com.example.demoqispaceapp

import android.app.Activity
import android.content.Intent
import android.os.Bundle
import android.util.Log
import android.widget.Button
import android.widget.TextView
import java.util.*

import android.graphics.Color
import kotlin.random.Random

import android.widget.RadioButton
import android.widget.RadioGroup

import android.text.SpannableStringBuilder
import android.text.Spanned
import android.text.style.ForegroundColorSpan

/**
 * This page demonstrates a use of the MASQ DS API to sign and verify a message between Alice and Bob.
 * Additionally, Charles attempts to verify the message and signature but fails.
 *
 * Logical Flow:
 * 1. Select the NIST security level you want to run the demo.
 * 2. Press 'Alice KEYPAIR' to generate keypair for Alice. It first initialized KEMs for Alice, Bob and Charles and also generated keypair for Charles.
 * 3. Press 'Alice SIGN' to let Alice sign a message with her private key.
 * 4. Press 'Bob VERIFY' to let Bob verify the authenticity of the message and signature using Alice’s public key.
 * 5. Press 'Charles ATTEMPTS TO VERIFY' to let Charles attempt to verify the authenticity of the message and its signature using his public key.
**/

class masqDsActivity : Activity() {
    private lateinit var sdk_demo_msg: TextView
    private lateinit var step1_msg: TextView
    private lateinit var step2_msg: TextView
    private lateinit var step3_msg: TextView
    private lateinit var step4_msg: TextView

    private val masqDs = masqDsKotlinWrapper()

    private var alicePk: ByteArray? = null
    private var charlesPk: ByteArray? = null

    private var aliceSk: ByteArray? = null
    private var charlesSk: ByteArray? = null

    private var aliceSign: ByteArray = byteArrayOf()
    private var charlesSign: ByteArray = byteArrayOf()

    private var aliceMsgString: String = "Hello world!"
    private var aliceMsg: ByteArray? = null

    private var aliceDsHandle: Long = 0L
    private var bobDsHandle: Long = 0L
    private var charlesDsHandle: Long = 0L

    var TextView.statusMessage: String
        get() = text.toString().removePrefix("👉 ")
        set(value) { text = "👉 $value" }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.masqds_layout)

        // Initialize UI elements
        sdk_demo_msg = findViewById(R.id.sdk_demo_msg)
        step1_msg = findViewById(R.id.step1_msg)
        step2_msg = findViewById(R.id.step2_msg)
        step3_msg = findViewById(R.id.step3_msg)
        step4_msg = findViewById(R.id.step4_msg)

        // MASQ DS demonstration buttons
        findViewById<Button>(R.id.button_step1_alice_generate_keypair).setOnClickListener {
            aliceGenKeypair()
        }

        findViewById<Button>(R.id.button_step2_alice_signs_msg).setOnClickListener {
            aliceSignsMsg()
        }

        findViewById<Button>(R.id.button_step3_bob_verifies_the_msg).setOnClickListener {
            bobVerifiesMsg()
        }

        findViewById<Button>(R.id.button_step4_charles_verifies_the_msg).setOnClickListener {
            charlesVerifiesMsg()
        }

        // Back button
        findViewById<Button>(R.id.button_back).setOnClickListener {
            cleanupDS()
            val toLanding = Intent(this, MainActivity::class.java)
            startActivity(toLanding)
        }

        var algoLevel = 1
        findViewById<RadioGroup>(R.id.radioGroup_dsLevel).setOnCheckedChangeListener { group, checkedId ->
            when (checkedId) {
                R.id.radio_dsL1 -> algoLevel = 1
                R.id.radio_dsL3 -> algoLevel = 3
                R.id.radio_dsL5 -> algoLevel = 5
            }
            initializeDS(algoLevel)
            seedDs()
            charlesGenKeypair()

            //if any, clean-up previous demo run

            alicePk = null; aliceSk = null
            aliceSign = byteArrayOf()
            listOf(step1_msg, step2_msg, step3_msg, step4_msg).forEach { it.text = "" }

            sdk_demo_msg.setTextColor(Color.GREEN)
            sdk_demo_msg.statusMessage = "Algorithm to use was set to LEVEL ${algoLevel}."
            Log.d("DEMO - MASQ_DS", "[ALGO] ---------------------------------------------------------------------------------------")
            Log.d("MASQ_DS", "[ALGO] Algorithm was set to Level $algoLevel")

        }

        // Set default selection – triggers listener automatically
        findViewById<RadioButton>(R.id.radio_dsL1).isChecked = true

    }


    private fun initializeDS(algoLevel: Int) {
        Log.d("DEMO - MASQ_DS", "[INIT] ---------------------------------------------------------------------------------------")
        try {
            // Initialize with Algo level for Alice, Bob and Charles
            aliceDsHandle = masqDs.masqDsQeepInit(algoLevel)
            bobDsHandle = masqDs.masqDsQeepInit(algoLevel)
            charlesDsHandle = masqDs.masqDsQeepInit(algoLevel)

            if (aliceDsHandle != 0L && bobDsHandle != 0L && charlesDsHandle != 0L) { // not NULL
                Log.d("DEMO - MASQ_DS", "[INIT] Alice's API return code: ${aliceDsHandle}")
                Log.d("DEMO - MASQ_DS", "[INIT] Bob's API return code: ${bobDsHandle}")
                Log.d("DEMO - MASQ_DS", "[INIT] Charles' API return code: ${charlesDsHandle}")
            } else {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Failed to initialize Alice's, Bob's, and Charles' DS handles!"
                Log.e("DEMO - MASQ_DS", "[INIT] API return code [Alive|Bob|Charles]: [${aliceDsHandle}|${bobDsHandle}|${charlesDsHandle}]")
                cleanupDS()
            }
        } catch (e: Exception) {
            sdk_demo_msg.setTextColor(Color.RED)
            sdk_demo_msg.statusMessage = "Exception error occurred while initializing Alice's, Bob's, and Charles' DS handles!"
            Log.e("DEMO - MASQ_DS", "[INIT] Exception error: <${e.message}>")
            e.printStackTrace()
            cleanupDS()
        }
    }


    private fun seedDs() {
        Log.d("DEMO - MASQ_DS", "[SEED] ---------------------------------------------------------------------------------------")
        try {
            val aliceSeedString = "01234567890123456789"
            val aliceSeed = aliceSeedString.toByteArray()
            val aliceSeedResult = masqDs.masqDsSeed(aliceDsHandle, aliceSeed, aliceSeed.size)

            val bobSeedString = "0123456789012345678901234567890"
            val bobSeed = bobSeedString.toByteArray()
            val bobSeedResult = masqDs.masqDsSeed(bobDsHandle, bobSeed, bobSeed.size)

            val charlesSeedString = "01234567890123456789123456789"
            val charlesSeed = charlesSeedString.toByteArray()
            val charlesSeedResult = masqDs.masqDsSeed(charlesDsHandle, charlesSeed, charlesSeed.size)

            if (aliceSeedResult == 0 && bobSeedResult == 0 && charlesSeedResult == 0) {
                Log.d("DEMO - MASQ_DS", "[SEED] Alice's API return code: ${aliceSeedResult}")
                Log.d("DEMO - MASQ_DS", "[SEED] Alice's seed len: ${aliceSeed.size}")
                Log.d("DEMO - MASQ_DS", "[SEED] Alice's seed used (str): ${aliceSeedString}")
                Log.d("DEMO - MASQ_DS", "[SEED] Alice's seed used (hex): ${byteArrayToString(aliceSeed)}")

                Log.d("DEMO - MASQ_DS", "[SEED] Bob's API return code: ${bobSeedResult}")
                Log.d("DEMO - MASQ_DS", "[SEED] Bob's seed len: ${bobSeed.size}")
                Log.d("DEMO - MASQ_DS", "[SEED] Bob's seed used (str): ${bobSeedString}")
                Log.d("DEMO - MASQ_DS", "[SEED] Bob's seed used (hex): ${byteArrayToString(bobSeed)}")

                Log.d("DEMO - MASQ_DS", "[SEED] Charles' API return code: ${charlesSeedResult}")
                Log.d("DEMO - MASQ_DS", "[SEED] Charles' seed len: ${charlesSeed.size}")
                Log.d("DEMO - MASQ_DS", "[SEED] Charles' seed used (str): ${charlesSeedString}")
                Log.d("DEMO - MASQ_DS", "[SEED] Charles' seed used (hex): ${byteArrayToString(charlesSeed)}")
            } else {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Alice’s, Bob's, and Charles' seeding operation failed!"
                Log.e("DEMO - MASQ_DS", "[SEED] API return code for [Alice|Bob|Charles]: [${aliceSeedResult}|${bobSeedResult}|${charlesSeedResult}]")
            }
        } catch (e: Exception) {
            sdk_demo_msg.setTextColor(Color.RED)
            sdk_demo_msg.statusMessage = "Exception error occurred while initializing Alice's, Bob's, and Charles' DS seed operation!"
            Log.e("DEMO - MASQ_DS", "[SEED] Exception error: <${e.message}>")
            e.printStackTrace()
            cleanupDS()
        }
    }


    private fun charlesGenKeypair() {
        try {
            if (charlesDsHandle == 0L) {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Charles' DS handle is not initialized!"
                return
            }

            Log.d("DEMO - MASQ_DS", "[KEYPAIR] -----------------------------------------------------------------------------------")

            val publicKeyLengthCharles = masqDs.masqDsPublicKeyLength(charlesDsHandle)
            val secretKeyLengthCharles = masqDs.masqDsPrivateKeyLength(charlesDsHandle)

            charlesPk = ByteArray(publicKeyLengthCharles)
            charlesSk = ByteArray(secretKeyLengthCharles)

            val resultCharles = masqDs.masqDsKeypair(charlesDsHandle, charlesPk!!, charlesSk!!)
            if (resultCharles == 0) {
                Log.d("DEMO - MASQ_DS", "[KEYPAIR] Charles' API return code: ${resultCharles}")
                Log.d("DEMO - MASQ_DS", "[KEYPAIR] Charles' pkLen: ${publicKeyLengthCharles}")
                Log.d("DEMO - MASQ_DS", "[KEYPAIR] Charles' skLen: ${secretKeyLengthCharles}")
                Log.d("DEMO - MASQ_DS", "[KEYPAIR] Charles' pk: ${byteArrayToString(charlesPk!!)}")
                Log.d("DEMO - MASQ_DS", "[KEYPAIR] Charles' sk: ${byteArrayToString(charlesSk!!)}")
            } else {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Failed to generate keypair!"
                Log.e("DEMO - MASQ_DS", "[KEYPAIR] API return code: $resultCharles")
                Log.e("DEMO - MASQ_DS", "[KEYPAIR] Keypair generation failed.")
            }
        } catch (e: Exception) {
            sdk_demo_msg.setTextColor(Color.RED)
            sdk_demo_msg.statusMessage = "An exception occurred while generating the keypair!"
            Log.e("DEMO - MASQ_DS", "[KEYPAIR] Exception error: <${e.message}>")
            e.printStackTrace()
        }
    }



    private fun aliceGenKeypair() {
        try {
            if (aliceDsHandle == 0L) {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Alice's DS handle is not initialized!"
                return
            }

            Log.d("DEMO - MASQ_DS", "[KEYPAIR] -----------------------------------------------------------------------------------")

            //if any, clean-up previous demo run
            listOf(step1_msg, step2_msg, step3_msg, step4_msg).forEach { it.text = "" }

            val pkLen = masqDs.masqDsPublicKeyLength(aliceDsHandle)
            val skLen = masqDs.masqDsPrivateKeyLength(aliceDsHandle)

            alicePk = ByteArray(pkLen)
            aliceSk = ByteArray(skLen)

            val result = masqDs.masqDsKeypair(aliceDsHandle, alicePk!!, aliceSk!!)

            if (result == 0 ) {
                step1_msg.text = "pk: ${byteArrayToString(alicePk!!).let { if (it.length <= 12) it else it.take(5) + "..." + it.takeLast(5) }}  |  sk: ${byteArrayToString(aliceSk!!).let { if (it.length <= 12) it else it.take(5) + "..." + it.takeLast(5) }}"
                sdk_demo_msg.setTextColor(Color.GREEN)
                sdk_demo_msg.statusMessage = "Alice successfully generated her public-private key pair. She is set to sign her message now."
                Log.d("DEMO - MASQ_DS", "[KEYPAIR] Alice's API return code for Alice: ${result}")
                Log.d("DEMO - MASQ_DS", "[KEYPAIR] Alice's pkLen: ${pkLen}")
                Log.d("DEMO - MASQ_DS", "[KEYPAIR] Alice's skLen: ${skLen}")
                Log.d("DEMO - MASQ_DS", "[KEYPAIR] Alice's pk: ${byteArrayToString(alicePk!!)}")
                Log.d("DEMO - MASQ_DS", "[KEYPAIR] Alice's sk: ${byteArrayToString(aliceSk!!)}")
            } else {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Failed to generate keypair!"
                Log.e("DEMO - MASQ_DS", "[KEYPAIR] API return code: $result")
                Log.e("DEMO - MASQ_DS", "[KEYPAIR] Keypair generation failed.")
            }
        } catch (e: Exception) {
            sdk_demo_msg.setTextColor(Color.RED)
            sdk_demo_msg.statusMessage = "An exception occurred while generating the keypair!"
            Log.e("DEMO - MASQ_DS", "[KEYPAIR] Exception error: <${e.message}>")
            e.printStackTrace()
        }
    }

    private fun aliceSignsMsg() {
        try {
            if (aliceDsHandle == 0L) {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Bob's DS handle is not initialized!"
                return
            }

            if (aliceSk == null) {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Alice’s private key is not available yet. A public–private key pair must be generated first."
                return
            }

            Log.d("DEMO - MASQ_DS", "[SIGN] -----------------------------------------------------------------------------------")

            //if any, clean-up previous demo run
            listOf(step3_msg, step4_msg).forEach { it.text = "" }

            val signLengthAlice = masqDs.masqDsSignatureLength(aliceDsHandle)
            aliceSign = ByteArray(signLengthAlice)

            aliceMsg = aliceMsgString.toByteArray()

            val result = masqDs.masqDsSign(aliceDsHandle, aliceSk!!, aliceMsg!!, aliceMsg!!.size, aliceSign, signLengthAlice)
            if (result == 0) {
                step2_msg.text = "Message: \"${aliceMsgString}\"\nAlice's digital signature: ${base64Encode(
                    aliceSign
                ).let { if (it.length <= 12) it else it.take(5) + "..." + it.takeLast(5) }}"
                sdk_demo_msg.setTextColor(Color.GREEN)
                sdk_demo_msg.statusMessage = "Alice successfully signs her message with her private key. She will send the message, its signature, and her public key to Bob."
                Log.d("DEMO - MASQ_DS", "[SIGN] Alice's API return code: ${result}")
                Log.d("DEMO - MASQ_DS", "[SIGN] Alice's msgLen: ${aliceMsg!!.size}")
                Log.d("DEMO - MASQ_DS", "[SIGN] Alice's msg (str): ${aliceMsgString}")
                Log.d("DEMO - MASQ_DS", "[SIGN] Alice's msg (hex): ${byteArrayToString(aliceMsg!!)}")
                Log.d("DEMO - MASQ_DS", "[SIGN] Alice's signLen: ${signLengthAlice}")
                Log.d("DEMO - MASQ_DS", "[SIGN] Alice's signature: ${base64Encode(aliceSign)}")
            } else {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Signing the message failed!"
                Log.e("DEMO - MASQ_DS", "[SIGN] API return code: $result")
                Log.e("DEMO - MASQ_DS", "[SIGN] Signing the message failed!")
            }
        } catch (e: Exception) {
            sdk_demo_msg.setTextColor(Color.RED)
            sdk_demo_msg.statusMessage = "An exception was thrown during the message signing process."
            Log.e("DEMO - MASQ_DS", "[SIGN] Exception error: <${e.message}>")
            e.printStackTrace()
        }
    }

    private fun bobVerifiesMsg() {
        try {
            if (aliceDsHandle == 0L) {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Alice's DS handle is not initialized!"
                return
            }

            if (bobDsHandle == 0L) {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Bob's DS handle is not initialized!"
                return
            }

            if (aliceSign.isEmpty()) {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Alice hasn't shared the digital signature of her message with Bob yet. Please complete the digital signing step first."
                return
            }

            if (alicePk == null) {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Alice’s public key is not available yet. A public–private key pair must be generated first."
                return
            }

            Log.d("DEMO - MASQ_DS", "[VER] --------------------------------------------------------------------------------------")

            //if any, clean-up previous demo run
            step4_msg.text = ""

            val signLengthAlice = masqDs.masqDsSignatureLength(aliceDsHandle)

            val result = masqDs.masqDsVerify(bobDsHandle, alicePk!!, aliceMsg!!, aliceMsg!!.size, aliceSign, signLengthAlice )
            if (result == 0) {
                sdk_demo_msg.setTextColor(Color.GREEN)
                sdk_demo_msg.statusMessage = "Bob successfully verifies the digital signature of Alice's message using her public key."

                step3_msg.text = SpannableStringBuilder(
                    "Alice's msg: \"$aliceMsgString\"\nAlice's digital signature: ${
                        base64Encode(aliceSign).let { if (it.length <= 12) it else it.take(5) + "..." + it.takeLast(5) }
                    }\nAlice's public key: ${
                        byteArrayToString(alicePk!!).let { if (it.length <= 12) it else it.take(5) + "..." + it.takeLast(5) }
                    }")

                Log.d("DEMO - MASQ_DS", "[VER] Bob's API return code: ${result}")
                Log.d("DEMO - MASQ_DS", "[VER] Alice's msgLen: ${aliceMsg!!.size}")
                Log.d("DEMO - MASQ_DS", "[VER] Alice's msg: ${byteArrayToString(aliceMsg!!)}")
                Log.d("DEMO - MASQ_DS", "[VER] Alice's signLen: $signLengthAlice")
                Log.d("DEMO - MASQ_DS", "[VER] Alice's signature: ${base64Encode(aliceSign)}")
            } else {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Verification of the signed message failed!"
                Log.e("DEMO - MASQ_DS", "[VER] API return code: $result")
                Log.e("DEMO - MASQ_DS", "[VER] Verification of the signed message failed!")
            }
        } catch (e: Exception) {
            sdk_demo_msg.setTextColor(Color.RED)
            sdk_demo_msg.statusMessage = "An exception was thrown during the message verification process."
            Log.e("DEMO - MASQ_DS", "[VER] Exception error: <${e.message}>")
            e.printStackTrace()
        }
    }

    private fun charlesVerifiesMsg() {
        try {
            if (charlesDsHandle == 0L) {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Charles' DS handle is not initialized!"
                return
            }

            if (aliceSign.isEmpty()) {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Alice hasn't shared the digital signature of her message with Charles yet. Please complete the digital signing step first."
                return
            }

            if (charlesPk == null) {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Charles' public key is not initialized!"
                return
            }

            Log.d("DEMO - MASQ_DS", "[VER] --------------------------------------------------------------------------------------")

            //if any, clean-up previous demo run
            step4_msg.text = ""

            val signLengthAlice = masqDs.masqDsSignatureLength(aliceDsHandle)

            val result = masqDs.masqDsVerify(charlesDsHandle, charlesPk!!, aliceMsg!!, aliceMsg!!.size, aliceSign, signLengthAlice )

            step4_msg.text = SpannableStringBuilder("Charles' public key: ${byteArrayToString(charlesPk!!).let { if (it.length <= 12) it else it.take(5) + "..." + it.takeLast(5) }}")

            if (result == 0) {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Charles was able to verify Alice's message & digital signature using his public key — this shouldn't happen!"

                Log.d("DEMO - MASQ_DS", "[VER] Charles' API return code: ${result}")
                Log.d("DEMO - MASQ_DS", "[VER] Alice's msgLen: ${aliceMsg!!.size}")
                Log.d("DEMO - MASQ_DS", "[VER] Alice's msg: ${byteArrayToString(aliceMsg!!)}")
                Log.d("DEMO - MASQ_DS", "[VER] Alice's signLen: $signLengthAlice")
                Log.d("DEMO - MASQ_DS", "[VER] Alice's signature: ${base64Encode(aliceSign)}")

            } else {
                sdk_demo_msg.setTextColor(Color.parseColor("#FFA500"))
                sdk_demo_msg.statusMessage = "Charles's verification of the message & digital signature failed — this is expected!"
                Log.e("DEMO - MASQ_DS", "[VER] API return code: $result")
                Log.e("DEMO - MASQ_DS", "[VER] Charles's verification of the message & digital signature failed — this is expected!")
            }
        } catch (e: Exception) {
            sdk_demo_msg.setTextColor(Color.RED)
            sdk_demo_msg.statusMessage = "An exception was thrown during the message verification process."
            Log.e("DEMO - MASQ_DS", "[VER] Exception error: <${e.message}>")
            e.printStackTrace()
        }
    }


    private fun cleanupDS() {
        if (aliceDsHandle != 0L) {
            try {
                masqDs.masqDsQeepFree(aliceDsHandle)
                aliceDsHandle = 0L
            } catch (e: Exception) {
                Log.e("DEMO - MASQ_DS", "Error freeing Alice's ds handle: ${e.message}")
            }
        }

        if (bobDsHandle != 0L) {
            try {
                masqDs.masqDsQeepFree(bobDsHandle)
                bobDsHandle = 0L
            } catch (e: Exception) {
                Log.e("DEMO - MASQ_DS", "Error freeing Bob's ds handle: ${e.message}")
            }
        }

        if (charlesDsHandle != 0L) {
            try {
                masqDs.masqDsQeepFree(charlesDsHandle)
                charlesDsHandle = 0L
            } catch (e: Exception) {
                Log.e("DEMO - MASQ_DS", "Error freeing Charles's ds handle: ${e.message}")
            }
        }
    }

    private fun base64Encode(data: ByteArray): String {
        return Base64.getEncoder().encodeToString(data)
    }

    override fun onDestroy() {
        cleanupDS()
        super.onDestroy()
    }

    private fun byteArrayToString(input : ByteArray) : String{
        var decode_string : String = ""
        for(b in 0..input.size-1){
            decode_string += String.format("%x", input[b])
        }
        return decode_string
    }
}