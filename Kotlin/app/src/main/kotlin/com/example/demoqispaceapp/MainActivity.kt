package com.example.demoqispaceapp

import android.content.Context
import android.content.Intent
import android.os.Build.VERSION_CODES
import android.os.Bundle
import android.view.View
import android.view.inputmethod.InputMethodManager
import android.widget.Button
import android.widget.EditText
import android.widget.TextView
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AppCompatActivity
import com.android.volley.Request
import com.android.volley.RequestQueue
import com.android.volley.Response
import com.android.volley.toolbox.JsonObjectRequest
import com.android.volley.toolbox.Volley
import org.json.JSONObject
import java.nio.charset.StandardCharsets
import java.util.*

/**
 * Currently, MainActivity is the landing page to navigate to 3 other pages:
 *   a. qeepActivity: handles QEEP operation and related QiSpace API for retrieving QK -> get QK, encrypt and decrypt
 *   b. sequrActivity: handles SEQUR operations and related Qispace API for retrieving QE -> setup subkey, get QE and get random numbers
 *   c. masqKemActivity: handles MASQ KEM operations -> generate keypair, encaps, decaps
 *   d. masqDsActivity: handles MASQ DS operations -> generate keypair, sign the message, verify the message
 */

class MainActivity : AppCompatActivity() {

    // define the sub key and token globally since app client and server should have the same copies at all times
    // Assume token is given for this demo app
    // For real applications, it is initialized during application setup

    companion object {
    // Helper function to convert hexString to byte array
        fun String.decodeHex(): ByteArray {
            check(length % 2 == 0) { "Must have an even length" }
            return chunked(2)
                .map { it.toInt(16).toByte() }
                .toByteArray()
        }

        // Note: token and subkey must be known and common between QEEP and SEQUR page.
        // sub_key is assumed to demonstrate it being already initialized on device setup, but here, it will need to be re-setup since current one is outdated
        public val token = "Bearer "+QiSpace_Info.QISPACE_ENTERPRISE_ACCESS_TOKEN
        public var sub_key: ByteArray = "016046e88c934d23f678f2095d41fcfbd4c6a97dfeeff23f9b613bb665148bcace20a65fff763a0a6714120afec6b8c61f86f9c95602e402343ba05efd5242ec052b26b0625393fbbe7317f368277c8f45c8eb9e94b35b16745aa8132859ebbbe58f6d71ed379b83b8881bdf4d07912e6126b149f01bc7c2d9a2e77aa9f286f8fa46e8742a788f63a25f412ef8ad72295b6f7c2fb3f47ba5dd6ba0459d13912899c65008936e3a7ca83b5f0c731a50ce0f8a4b7872e015e2c3f8774ff0f675beb0eeed723dd2eaeaaf45fa3ec1cfd9dd16be3b75ddbe1d7f0b131269da26597c2c5a5555f4311a76fb0edea703fe42d45020a7461a8afc9243638bf881d436a661df51c3b7de76a1c2574425d582d3610acb8872d095bbaa22fcb9d2559f015d896d1005918ba559e75e9c3694e3feb7c1d41da297cc94169643fa6c519025ce0df2db7f1f4f6fe6b546a0d9df84fb08048c908e2480d146a49034ea1e1414d9747be856cd3143b809c33b18c131dccc5a918c2c49ea0d28eab4152422a04163d55f480db238290638bc9255a5170805c14ba5bb7cd07e92c15ee51de65dc6e49e678cb248423ffa9df2830e4ba2f15c1bd666788ed7a0f2ce89630ece66d01c02acc4f21ebbee5eadf0c93ca5ad7332ed190000188ca6a985e8836a0db550d3d1458f7bc211b05241ea35573156a6f405549bcb6b499952f104bbc9c39bdfaeccb2a438b773f7457e645d7f23a336729770819fafa4ac2badb4bfe1aae4ef2b59818f4279e90096b4fe95b1d9e03f0c1435ae88c71de25fd01ce71f61d7b034e6a2504b4b13dadf77313f69774e2fa3bf07c66dc9216b705f99b10b23cacb9a07872f5f12dd0519c07beafa485414fb115b0cd7a84bcc2399657d2f792c71c76ecba8ac327151b9d59ada4a1845bfe561f1b29c5484c97912c16dc331da2869d06f49eaa3ade390921ad2bfc6f8ff3fb63ae3a0ba880551faf17b16fe2eca42b91e101371397b21da6e9ec2c43ac96af357067a101bef478837179e54597cfcd9138acf03bf614c6d6ada65642c35bb74a9cfc4384462349092d673e6281a0d4b9e91e86df0ad80d8b310cd6315b538e0bf193b12b487d03fa777edaa129d37922571e19c3efa45c526d1daf62d49689a64b406806938d819fb77e4803733765bb28b7909d7022a76dda679b9f017898d5d66cc4167be562840110eb97e04856b0e64eb97f4feb63b262cc71dbca0bba29d3a681e1ab3ac7032ceb657042a7439ba35155706a0708d61ef386bf4631f75efbae50f45bc99faab0e9ff9d796dd48cc2ec7df23265bc300b47b745b7aef2cdf6c6e56c58f28cb7d1a552b0e17da2b33254a7ca2e8a00eb4c90d69c72d78ef4a4e8b06bc5abdc736e271c1d00c36937de9a6086fe078255b17140230749c2098b980e467d27f77a730f7053eb2dc45cb73d1add5adec54ba28aeeae6da28cef2474b190c".decodeHex()
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // BUTTON: Sends user to SEQUR demo page
        findViewById<Button>(R.id.button_to_sequr).setOnClickListener{
            val toSequr = Intent(this, sequrActivity::class.java)
            startActivity(toSequr)
        }

        // BUTTON: Sends user to QEEP demo page
        findViewById<Button>(R.id.button_to_qeep).setOnClickListener{
            val toQeep = Intent(this, qeepActivity::class.java)
            startActivity(toQeep)
        }

        // BUTTON: Sends user to MASQ-KEM demo page
        findViewById<Button>(R.id.button_to_masq_kem).setOnClickListener{
            val toMasqKem = Intent(this, masqKemActivity::class.java)
            startActivity(toMasqKem)
        }

        // BUTTON: Sends user to MASQ-DS demo page
        findViewById<Button>(R.id.button_to_masq_ds).setOnClickListener{
            val toMasqDs = Intent(this, masqDsActivity::class.java)
            startActivity(toMasqDs)
        }

    }
}
