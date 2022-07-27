package com.example.qispace_sdk_java_demo;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;

/**
* Currently, MainActivity is the landing page to navigate to 3 other pages:
*    a. QeepActivity: handles QEEP operation and related QiSpace API for retrieving QK -> get QK, encrypt and decrypt
*    b. SequrActivity handles SEQUR operation and related QiSpace API for retrieving QE -> setup subkey, get QE and get random numver
*    c. MasqActivity: coming soon...
*/

public class MainActivity extends AppCompatActivity {

    // define the sub_key and token globally since app client and server should have the same copies at all times
    // Assume token is given for this demo app
    // For real applications, it is initialized during application setup

    public static byte[] sub_key = hexStringToByteArray("0162b30a0f2fc74b148a910cdf2cb8e567d35d852b221d1629b7f8aa9d825ebd28f934b39bd9908eecf85af5922425d45b52475f16516f740ae3cea57ae25c1f1aa089ccd95cc1a2c7b6eb7f7f54834a9dcfe51d385cadf53d356fb3836123544094b74951eea9087aa362580724493e02ce494e356474984fe6b7db8b298beb22153a40613e5879bdcbc4bfbd76ab134b66cca9e44968f82870bc68f395fbad56162a685c3519f1563e5db0077a9acd8afd40dc36687a4ec3322570c819855f235d81f59706cf2eaaaf8d08206269b5a518e100ddea1873d9ff92a3ad3c6a006d4cf21f3ce404f8b6d5a986b0e8c14bcd1eeafad18115028db478d0678fa2deaf17bc68ddff014a1c44250fb7e56eca0c32c56411bd74a17778ada741456101217eb7c1d480e53a10f66106fd2b0147111e6d655c56c47c37065f20791adb644f09ed287e9e4c448684982dc42a4b821cf24fd30a4120249d1e491d9f69dfc40a14105ef75bba6ec8eb4b560f9285297a6b9f2d999572d0724084a32ea3373061529c8a96bac56d5dbf03dc9fe7f89f36b7803b6b32b6ffbbbe1dccb543a0ec1e74824d196e8eebfcb7019c21037c2143426f7ee86d4416bbfb2cb545c730b8ce88457bdf60cb65e95e4012d32f484bbff77f63953bfc457dd5e2c964496e7964abf9d6de92a487fe7315e96c58ed368e331b3dde138165fbd4a4c833dd6efe273e26661c4aa11ab64d498f1e869f01ad3f444634b2bea60b5061aaad7d23897a45828958f73d3cca11fedaac0623a7293cea8cd3b887f73a0ba813e44198795a5c27c18079619e656521585b9df98f7066bf770e81d373434454b107d3b0598b65ff0cd0c41f3ba5ee29f7b888d978eb8d41acde6d5f610a986e99f6691185b58c10b231b5a1042299f3a4a64cd839a1e1e3895b51fb2492e63d26a645c0d49603025dc2981e020f8194352d0291dad0eebd9b88267cb4531997919775864a741efda44176c9588d33d56438e9b83d9fe0ca632c33a47df93d022c328b81fdf774f94a37cec67d7f7720d07e8c969bf2f4e08de2c86d2e5d4d393320afc6a5998aafadd38d7e37b0fb69706d6324dd542f04a60ede337ca92b918ec6c19fa9e87910cb6858077011f4b9ab4c80788c08499869999f704436d69c1d60938dac666b5c3c80373a9298c1879fe0eceb783d6e13047ec812ad8f873c5fac512c3accda7a3b811709af4c8eda0446d3818bdff9ee9440402c3cca952412f9e3f908911b5383e4dfdf1f4cf3bf6fd277b12cc045768dce85220716765f4987aa9045734ef9d93add8cf3080aeca70707d59c137df47b2402bcd75e0d921b99955d5181b9e0a0796357deaaf769aaa215e27c031d25460bb935409ae047ad53c69240eccedb195d539a05d59558cec1ba5855e5b708334e71a03dfcbc8da4a6c7549b4c3c1a750aca7b0483b649fa091178568e917353457f");
    public static String token = "Bearer "+QiSpace_Info.QISPACE_ENTERPRISE_ACCESS_TOKEN;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // BUTTON: Sends user to SEQUR demo page
        findViewById(R.id.button_to_sequr).setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View view) {
                Intent goToSEQUR = new Intent(MainActivity.this, SequrActivity.class);
                startActivity(goToSEQUR);
            }
        });

        // BUTTON: Sends user to QEEP demo page
        findViewById(R.id.button_to_qeep).setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View view) {
                Intent goToQEEP = new Intent(MainActivity.this, QeepActivity.class);
                startActivity(goToQEEP);
            }
        });

        // BUTTON: Sends user to MASQ demo page
        findViewById(R.id.button_to_masq).setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View view) {
                Intent goToMASQ = new Intent(MainActivity.this, MasqActivity.class);
                startActivity(goToMASQ);
            }
        });
    }

    // Helper function to convert hexString to byte array
    // Used to convert the pre-generated key, which is inputted as a hex string, into a byte array
    private static byte[] hexStringToByteArray(String s) {
        int len = s.length();
        byte[] data = new byte[len / 2];
        for (int i = 0; i < len; i += 2) {
            data[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4)
                    + Character.digit(s.charAt(i+1), 16));
        }
        return data;
    }

}