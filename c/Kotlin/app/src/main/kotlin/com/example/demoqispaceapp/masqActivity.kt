package com.example.demoqispaceapp

import android.app.Activity
import android.content.Intent
import android.os.Bundle
import android.widget.Button

class masqActivity : Activity() {
    override fun onCreate(savedInstanceState: Bundle?){
        super.onCreate(savedInstanceState)
        setContentView(R.layout.masq_layout)

        // BUTTON: Send user back to landing page
        findViewById<Button>(R.id.button_masq_back).setOnClickListener{
            val toLanding = Intent(this, MainActivity::class.java)
            startActivity(toLanding)
        }
    }

}