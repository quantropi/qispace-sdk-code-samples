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
 * This page demonstrates a use of the MASQ KEM API to encrypt and decrypt a shared secret between Alice and Bob.
 * Additionally, Charles attempts to capture the ciphertext and decapsulate it but fails.
 *
 * Logical Flow:
 * 1. Select the NIST security level you want to run the demo.
 * 2. Press 'Alice KEYPAIR' to generate keypair for Alice. It first initialized KEMs for Alice, Bob and Charles and also generated keypair for Charles.
 * 3. Press 'Bob ENCAPS' to let Bob encapsulate a shared secret using Alice's public key.
 * 4. Press 'Alice DECAPS' to let Alice decapsulate the ciphertext shared by Bob using her private key.
 * 5. Press 'Charles ATTEMPTS TO DECAPS' to let Charles decapsulate Bob's ciphertext using his own private key.
 */

class masqKemActivity : Activity() {
    private lateinit var sdk_demo_msg: TextView
    private lateinit var step1_msg: TextView
    private lateinit var step2_msg: TextView
    private lateinit var step3_msg: TextView
    private lateinit var step4_msg: TextView

    private val masqKem = masqKemKotlinWrapper()

    private var alicePk: ByteArray? = null
    private var charlesPk: ByteArray? = null

    private var aliceSk: ByteArray? = null
    private var charlesSk: ByteArray? = null

    private var bobSharedSecretKey: ByteArray = byteArrayOf()
    private var ct: ByteArray = byteArrayOf()

    private var aliceKemHandle: Long = 0L
    private var bobKemHandle: Long = 0L
    private var charlesKemHandle: Long = 0L

    var TextView.statusMessage: String
    get() = text.toString().removePrefix("👉 ")
    set(value) { text = "👉 $value" }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.masqkem_layout)

        // Initialize UI elements
        sdk_demo_msg = findViewById(R.id.sdk_demo_msg)
        step1_msg = findViewById(R.id.step1_msg)
        step2_msg = findViewById(R.id.step2_msg)
        step3_msg = findViewById(R.id.step3_msg)
        step4_msg = findViewById(R.id.step4_msg)

        // MASQ KEM demonstration buttons
        findViewById<Button>(R.id.button_step1_alice_generate_keypair).setOnClickListener {
            aliceGenKeypair()
        }

        findViewById<Button>(R.id.button_step2_bob_encapsulate).setOnClickListener {
            bobEncapsulate()
        }

        findViewById<Button>(R.id.button_step3_alice_decapsulate).setOnClickListener {
            aliceDecapsulate()
        }

        findViewById<Button>(R.id.button_step4_charles_decapsulate).setOnClickListener {
            charlesDecapsulate()
        }

        // Back button
        findViewById<Button>(R.id.button_back).setOnClickListener {
            cleanupKEM()
            val toLanding = Intent(this, MainActivity::class.java)
            startActivity(toLanding)
        }

        var algoLevel = 1
        findViewById<RadioGroup>(R.id.radioGroup_kemLevel).setOnCheckedChangeListener { group, checkedId ->
            when (checkedId) {
                R.id.radio_kemL1 -> algoLevel = 1 
                R.id.radio_kemL3 -> algoLevel = 3
                R.id.radio_kemL5 -> algoLevel = 5
            }

            initializeKEM(algoLevel)
            seedKem()
            charlesGenKeypair()

            //if any, clean-up previous demo run
            alicePk = null; aliceSk = null
            bobSharedSecretKey = byteArrayOf(); ct = byteArrayOf()
            listOf(step1_msg, step2_msg, step3_msg, step4_msg).forEach { it.text = "" }

            sdk_demo_msg.setTextColor(Color.GREEN)
            sdk_demo_msg.statusMessage = "Algorithm to use was set to LEVEL ${algoLevel}."
            Log.d("DEMO - MASQ_KEM", "[ALGO] ---------------------------------------------------------------------------------------")
            Log.d("MASQ_KEM", "[ALGO] Algorithm was set to Level $algoLevel")
        }

        // Set default selection – triggers listener automatically
        findViewById<RadioButton>(R.id.radio_kemL1).isChecked = true

    }


    private fun initializeKEM(algoLevel: Int) {
        Log.d("DEMO - MASQ_KEM", "[INIT] ---------------------------------------------------------------------------------------")
        try {
            // Initialize with Algo level for Alice, Bob and Charles
            aliceKemHandle = masqKem.masqKemQeepInit(algoLevel)
            bobKemHandle = masqKem.masqKemQeepInit(algoLevel)
            charlesKemHandle = masqKem.masqKemQeepInit(algoLevel)

            if (aliceKemHandle != 0L && bobKemHandle != 0L && charlesKemHandle != 0L) { // not NULL
                Log.d("DEMO - MASQ_KEM", "[INIT] Alice's API return code: ${aliceKemHandle}")
                Log.d("DEMO - MASQ_KEM", "[INIT] Bob's API return code: ${bobKemHandle}")
                Log.d("DEMO - MASQ_KEM", "[INIT] Charles' API return code: ${charlesKemHandle}")
            } else {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Failed to initialize Alice's, Bob's, and Charles' KEM handles!"
                Log.e("DEMO - MASQ_KEM", "[INIT] API return code [Alive|Bob|Charles]: [${aliceKemHandle}|${bobKemHandle}|${charlesKemHandle}]")
                cleanupKEM()
            }
        } catch (e: Exception) {
            sdk_demo_msg.setTextColor(Color.RED)
            sdk_demo_msg.statusMessage = "Exception error occurred while initializing Alice's, Bob's, and Charles' KEM handles!"
            Log.e("DEMO - MASQ_KEM", "[INIT] Exception error: <${e.message}>")
            e.printStackTrace()
            cleanupKEM()
        }
    }


   private fun seedKem() {
        Log.d("DEMO - MASQ_KEM", "[SEED] ---------------------------------------------------------------------------------------")
        try {
            val aliceSeedString = "0123456789ABCDEF0123456789GHIJKL"
            val aliceSeed = aliceSeedString.toByteArray()
            val aliceSeedResult = masqKem.masqKemSeed(aliceKemHandle, aliceSeed, aliceSeed.size)

            val bobSeedString = "0123456789ABCDEF0123456789GHIJKL01234567890"
            val bobSeed = bobSeedString.toByteArray()
            val bobSeedResult = masqKem.masqKemSeed(bobKemHandle, bobSeed, bobSeed.size)

            val charlesSeedString = "0123456789ABCDEF0123456789GHIJKL01234567890MNOPQR"
            val charlesSeed = charlesSeedString.toByteArray()
            val charlesSeedResult = masqKem.masqKemSeed(charlesKemHandle, charlesSeed, charlesSeed.size)

            if (aliceSeedResult == 0 && bobSeedResult == 0 && charlesSeedResult == 0) {
                Log.d("DEMO - MASQ_KEM", "[SEED] Alice's API return code: ${aliceSeedResult}")
                Log.d("DEMO - MASQ_KEM", "[SEED] Alice's seed len: ${aliceSeed.size}")
                Log.d("DEMO - MASQ_KEM", "[SEED] Alice's seed used (str): ${aliceSeedString}")
                Log.d("DEMO - MASQ_KEM", "[SEED] Alice's seed used (hex): ${byteArrayToString(aliceSeed)}")

                Log.d("DEMO - MASQ_KEM", "[SEED] Bob's API return code: ${bobSeedResult}")
                Log.d("DEMO - MASQ_KEM", "[SEED] Bob's seed len: ${bobSeed.size}")
                Log.d("DEMO - MASQ_KEM", "[SEED] Bob's seed used (str): ${bobSeedString}")
                Log.d("DEMO - MASQ_KEM", "[SEED] Bob's seed used (hex): ${byteArrayToString(bobSeed)}")

                Log.d("DEMO - MASQ_KEM", "[SEED] Charles' API return code: ${charlesSeedResult}")
                Log.d("DEMO - MASQ_KEM", "[SEED] Charles' seed len: ${charlesSeed.size}")
                Log.d("DEMO - MASQ_KEM", "[SEED] Charles' seed used (str): ${charlesSeedString}")
                Log.d("DEMO - MASQ_KEM", "[SEED] Charles' seed used (hex): ${byteArrayToString(charlesSeed)}")
            } else {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Alice’s, Bob's, and Charles' seeding operation failed!"
                Log.e("DEMO - MASQ_KEM", "[SEED] API return code for [Alice|Bob|Charles]: [${aliceSeedResult}|${bobSeedResult}|${charlesSeedResult}]")
            }
        } catch (e: Exception) {
            sdk_demo_msg.setTextColor(Color.RED)
            sdk_demo_msg.statusMessage = "Exception error occurred while initializing Alice's, Bob's, and Charles' KEM seed operation!"
            Log.e("DEMO - MASQ_KEM", "[SEED] Exception error: <${e.message}>")
            e.printStackTrace()
            cleanupKEM()
        }
    }


    private fun charlesGenKeypair() {
         try {
            if (charlesKemHandle == 0L) {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Charles' KEM handle is not initialized!"
                return
            }

            Log.d("DEMO - MASQ_KEM", "[KEYPAIR] -----------------------------------------------------------------------------------")

            val publicKeyLengthCharles = masqKem.masqKemPublicKeyLength(charlesKemHandle)
            val secretKeyLengthCharles = masqKem.masqKemSecretKeyLength(charlesKemHandle)
            charlesPk = ByteArray(publicKeyLengthCharles)
            charlesSk = ByteArray(secretKeyLengthCharles)

            val resultCharles = masqKem.masqKemKeypair(charlesKemHandle, charlesPk!!, charlesSk!!)
            if (resultCharles == 0) {
                Log.d("DEMO - MASQ_KEM", "[KEYPAIR] Charles' API return code: ${resultCharles}")
                Log.d("DEMO - MASQ_KEM", "[KEYPAIR] Charles' pkLen: ${publicKeyLengthCharles}")
                Log.d("DEMO - MASQ_KEM", "[KEYPAIR] Charles' skLen: ${secretKeyLengthCharles}")
                Log.d("DEMO - MASQ_KEM", "[KEYPAIR] Charles' pk: ${byteArrayToString(charlesPk!!)}")
                Log.d("DEMO - MASQ_KEM", "[KEYPAIR] Charles' sk: ${byteArrayToString(charlesSk!!)}")
            } else {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Failed to generate keypair!"
                Log.e("DEMO - MASQ_KEM", "[KEYPAIR] API return code: $resultCharles")
                Log.e("DEMO - MASQ_KEM", "[KEYPAIR] Keypair generation failed.")
            }
        } catch (e: Exception) {
            sdk_demo_msg.setTextColor(Color.RED)
            sdk_demo_msg.statusMessage = "An exception occurred while generating the keypair!"
            Log.e("DEMO - MASQ_KEM", "[KEYPAIR] Exception error: <${e.message}>")
            e.printStackTrace()
        }
    }



    private fun aliceGenKeypair() {
        try {
            if (aliceKemHandle == 0L) {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Alice's KEM handle is not initialized!"
                return
            }

            Log.d("DEMO - MASQ_KEM", "[KEYPAIR] -----------------------------------------------------------------------------------")

            //if any, clean-up previous demo run
            listOf(step1_msg, step2_msg, step3_msg, step4_msg).forEach { it.text = "" }
            bobSharedSecretKey = byteArrayOf(); ct = byteArrayOf()

            val pkLen = masqKem.masqKemPublicKeyLength(aliceKemHandle)
            val skLen = masqKem.masqKemSecretKeyLength(aliceKemHandle)
            alicePk = ByteArray(pkLen)
            aliceSk = ByteArray(skLen)

            val result = masqKem.masqKemKeypair(aliceKemHandle, alicePk!!, aliceSk!!)
            if (result == 0 ) {
                step1_msg.text = "pk: ${byteArrayToString(alicePk!!).let { if (it.length <= 12) it else it.take(5) + "..." + it.takeLast(5) }}  |  sk: ${byteArrayToString(aliceSk!!).let { if (it.length <= 12) it else it.take(5) + "..." + it.takeLast(5) }}"
                sdk_demo_msg.setTextColor(Color.GREEN)
                sdk_demo_msg.statusMessage = "Alice successfully generated her public-private key pair. She will share her public key with Bob."
                Log.d("DEMO - MASQ_KEM", "[KEYPAIR] Alice's API return code for Alice: ${result}")
                Log.d("DEMO - MASQ_KEM", "[KEYPAIR] Alice's pkLen: ${pkLen}")
                Log.d("DEMO - MASQ_KEM", "[KEYPAIR] Alice's skLen: ${skLen}")
                Log.d("DEMO - MASQ_KEM", "[KEYPAIR] Alice's pk: ${byteArrayToString(alicePk!!)}")
                Log.d("DEMO - MASQ_KEM", "[KEYPAIR] Alice's sk: ${byteArrayToString(aliceSk!!)}")
            } else {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Failed to generate keypair!"
                Log.e("DEMO - MASQ_KEM", "[KEYPAIR] API return code: $result")
                Log.e("DEMO - MASQ_KEM", "[KEYPAIR] Keypair generation failed.")
            }
        } catch (e: Exception) {
            sdk_demo_msg.setTextColor(Color.RED)
            sdk_demo_msg.statusMessage = "An exception occurred while generating the keypair!"
            Log.e("DEMO - MASQ_KEM", "[KEYPAIR] Exception error: <${e.message}>")
            e.printStackTrace()
        }
    }

    private fun bobEncapsulate() {
        try {
            if (bobKemHandle == 0L) {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Bob's KEM hanlde is not initialized!"
                return
            }

            if (alicePk == null) {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Alice’s public key is not available yet. A public–private key pair must be generated first." 
                return
            }

            Log.d("DEMO - MASQ_KEM", "[ENCAPS] -----------------------------------------------------------------------------------")

            //if any, clean-up previous demo run
            listOf(step3_msg, step4_msg).forEach { it.text = "" }

            val ctLen = masqKem.masqKemCiphertextLength(bobKemHandle)
            val sskLen = masqKem.masqKemSharedSecretLength(bobKemHandle)

            ct = ByteArray(ctLen)
            bobSharedSecretKey = ByteArray(sskLen) // { Random.nextInt(0, 256).toByte() }

            val result = masqKem.masqKemEncaps(bobKemHandle, alicePk!!, bobSharedSecretKey, ct)
            if (result == 0) {
                step2_msg.text = "Shared secret (ss): ${base64Encode(bobSharedSecretKey).let { if (it.length <= 12) it else it.take(5) + "..." + it.takeLast(5) }}\n Ciphertext: ${base64Encode(
                    ct
                ).let { if (it.length <= 12) it else it.take(5) + "..." + it.takeLast(5) }}"
                sdk_demo_msg.setTextColor(Color.GREEN)
                sdk_demo_msg.statusMessage = "Bob successfully encapsulated a shared secret. He will share the generated ciphertext with Alice for decapsulation."
                Log.d("DEMO - MASQ_KEM", "[ENCAPS] Bob's API return code: $result")
                Log.d("DEMO - MASQ_KEM", "[ENCAPS] Bob's ctLen: $ctLen")
                Log.d("DEMO - MASQ_KEM", "[ENCAPS] Bob's sskLen: $sskLen")
                Log.d("DEMO - MASQ_KEM", "[ENCAPS] Bob's ct: ${base64Encode(ct)}")
                Log.d("DEMO - MASQ_KEM", "[ENCAPS] Bob's shared secret: ${base64Encode(bobSharedSecretKey)}")
            } else {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Encapsulation of the shared secret failed!"
                Log.e("DEMO - MASQ_KEM", "[ENCAPS] API return code: $result")
                Log.e("DEMO - MASQ_KEM", "[ENCAPS] Encapsulation failed.")
            }
        } catch (e: Exception) {
            sdk_demo_msg.setTextColor(Color.RED)
            sdk_demo_msg.statusMessage = "An exception occurred during encapsulation!"
            Log.e("DEMO - MASQ_KEM", "[ENCAPS] Exception error: <${e.message}>")
            e.printStackTrace()
        }
    }

    private fun aliceDecapsulate() {
        try {
            if (aliceKemHandle == 0L) {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Alice's KEM handle is not initialized!"
                return
            }

            if (ct.isEmpty()) {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Bob has not shared the ciphertext with Alice yet. Please complete the encapsulation process first."
                return
            }

            if (aliceSk == null) {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Alice’s private key is not available yet. A public–private key pair must be generated first."
                return
            }

            Log.d("DEMO - MASQ_KEM", "[DECAPS] -----------------------------------------------------------------------------------")

            //if any, clean-up previous demo run
            step4_msg.text = ""

            val sskLen = masqKem.masqKemSharedSecretLength(aliceKemHandle)
            val aliceSharedSecretKey = ByteArray(sskLen)

            val result = masqKem.masqKemDecaps(aliceKemHandle, aliceSk!!, ct, aliceSharedSecretKey)
            if (result == 0) {
                sdk_demo_msg.setTextColor(Color.GREEN)
                sdk_demo_msg.statusMessage = "Alice successfully decapsulated Bob's ciphertext."

                step3_msg.text = base64Encode(aliceSharedSecretKey).let { if (it.length <= 12) it else it.take(5) + "..." + it.takeLast(5) }.let { sharedKey -> SpannableStringBuilder("Alice's obtained shared secret: $sharedKey").apply { setSpan(ForegroundColorSpan(Color.GREEN), indexOf(sharedKey), indexOf(sharedKey) + sharedKey.length, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE) } }
                step2_msg.text = base64Encode(bobSharedSecretKey).let { if (it.length <= 12) it else it.take(5) + "..." + it.takeLast(5) }.let { ssk -> base64Encode(
                    ct
                ).let { if (it.length <= 12) it else it.take(5) + "..." + it.takeLast(5) }.let { ctStr -> SpannableStringBuilder("Shared secret (ssk): $ssk\n Ciphertext: $ctStr").apply { setSpan(ForegroundColorSpan(Color.GREEN), indexOf(ssk), indexOf(ssk) + ssk.length, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE) } } }


                Log.d("DEMO - MASQ_KEM", "[DECAPS] Alice's API return code: $result")
                Log.d("DEMO - MASQ_KEM", "[DECAPS] Bob's ct: ${base64Encode(ct)}")
                Log.d("DEMO - MASQ_KEM", "[DECAPS] Alice's sskLen: $sskLen")
                Log.d("DEMO - MASQ_KEM", "[DECAPS] Alice's sk: ${byteArrayToString(aliceSk!!)}")
                Log.d("DEMO - MASQ_KEM", "[DECAPS] Alice's shared secret: ${base64Encode(aliceSharedSecretKey)}")

                // Verify that both parties have the same shared secret.
                val secretsMatch = aliceSharedSecretKey.contentEquals(bobSharedSecretKey)
                if (secretsMatch) {
                    sdk_demo_msg.setTextColor(Color.GREEN)
                    sdk_demo_msg.statusMessage = "Alice successfully decapsulated Bob's ciphertext.\nNow, Alice and Bob share the same secret!"
                } else {
                    sdk_demo_msg.setTextColor(Color.RED)
                    sdk_demo_msg.statusMessage = "Error! Shared secrets are not identical."
                }
            } else {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Decapsulation of the shared secret failed!"
                Log.e("DEMO - MASQ_KEM", "[DECAPS] API return code: $result")
                Log.e("DEMO - MASQ_KEM", "[DECAPS] Decapsulation failed.")
            }
        } catch (e: Exception) {
            sdk_demo_msg.setTextColor(Color.RED)
            sdk_demo_msg.statusMessage = "An exception occurred during decapsulation!"
            Log.e("DEMO - MASQ_KEM", "[DECAPS] Exception error: <${e.message}>")
            e.printStackTrace()
        }
    }

    private fun charlesDecapsulate() {
        try {
            if (charlesKemHandle == 0L) {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Charles' KEM handle is not initialized!"
                return
            }

            if (charlesSk == null) {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Charles’s private key is not initialized!"
                return
            }

            if (ct.isEmpty()) {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Bob has not shared the ciphertext with Charles yet. Please complete the encapsulation process first."
                return
            }

            Log.d("DEMO - MASQ_KEM", "[DECAPS] -----------------------------------------------------------------------------------")

            val sskLen = masqKem.masqKemSharedSecretLength(aliceKemHandle)
            val charlesSharedSecretKey = ByteArray(sskLen)

            val result = masqKem.masqKemDecaps(charlesKemHandle, charlesSk!!,
                ct, charlesSharedSecretKey)

            if (result == 0) {
                sdk_demo_msg.setTextColor(Color.GREEN)
                sdk_demo_msg.statusMessage = "Charles successfully decapsulated Bob's ciphertext."

                step4_msg.text = base64Encode(charlesSharedSecretKey).let { if (it.length <= 12) it else it.take(5) + "..." + it.takeLast(5) }.let { sharedKey -> SpannableStringBuilder("Charles' obtained shared secret: $sharedKey").apply { setSpan(ForegroundColorSpan(Color.RED), indexOf(sharedKey), indexOf(sharedKey) + sharedKey.length, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE) } }

                Log.d("DEMO - MASQ_KEM", "[DECAPS] Charles' API return code: $result")
                Log.d("DEMO - MASQ_KEM", "[DECAPS] Bob's ct: ${base64Encode(ct)}")
                Log.d("DEMO - MASQ_KEM", "[DECAPS] Charles' sskLen: $sskLen")
                Log.d("DEMO - MASQ_KEM", "[DECAPS] Charles' sk: ${byteArrayToString(charlesSk!!)}")
                Log.d("DEMO - MASQ_KEM", "[DECAPS] Charles' shared secret: ${base64Encode(charlesSharedSecretKey)}")

                // Verify that both parties have the same shared secret.
                val secretsMatch = charlesSharedSecretKey.contentEquals(bobSharedSecretKey)
                if (secretsMatch) {
                    sdk_demo_msg.setTextColor(Color.RED)
                    sdk_demo_msg.statusMessage = "Charles successfully decapsulated Bob's ciphertext — this shouldn't happen!\nCharles and Bob share the same secret."
                } else {
                    sdk_demo_msg.setTextColor(Color.parseColor("#FFA500"))
                    sdk_demo_msg.statusMessage = "Shared secret exchange failed  — this is expected!\nCharles and Bob do not share the same secret!"
                }
            } else {
                sdk_demo_msg.setTextColor(Color.RED)
                sdk_demo_msg.statusMessage = "Decryption of the shared secret failed!"
                Log.e("DEMO - MASQ_KEM", "[DECAPS] API return code: $result")
                Log.e("DEMO - MASQ_KEM", "[DECAPS] Decapsulation failed.")
            }
        } catch (e: Exception) {
            sdk_demo_msg.setTextColor(Color.RED)
            sdk_demo_msg.statusMessage = "An exception occurred during decryption!"
            Log.e("DEMO - MASQ_KEM", "[DECAPS] Exception error: <${e.message}>")
            e.printStackTrace()
        }
    }


    private fun cleanupKEM() {
        if (aliceKemHandle != 0L) {
            try {
                masqKem.masqKemQeepFree(aliceKemHandle)
                aliceKemHandle = 0L
            } catch (e: Exception) {
                Log.e("DEMO - MASQ_KEM", "Error freeing Alice's kem handle: ${e.message}")
            }
        }

        if (bobKemHandle != 0L) {
            try {
                masqKem.masqKemQeepFree(bobKemHandle)
                bobKemHandle = 0L
            } catch (e: Exception) {
                Log.e("DEMO - MASQ_KEM", "Error freeing Bob's kem handle: ${e.message}")
            }
        }

        if (charlesKemHandle != 0L) {
            try {
                masqKem.masqKemQeepFree(charlesKemHandle)
                charlesKemHandle = 0L
            } catch (e: Exception) {
                Log.e("DEMO - MASQ_KEM", "Error freeing Charles's kem handle: ${e.message}")
            }
        }
    }


    private fun base64Encode(data: ByteArray): String {
        return Base64.getEncoder().encodeToString(data)
    }

    override fun onDestroy() {
        cleanupKEM()
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