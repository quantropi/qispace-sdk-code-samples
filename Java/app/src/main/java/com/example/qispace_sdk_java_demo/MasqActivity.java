package com.example.qispace_sdk_java_demo;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;

import androidx.annotation.Nullable;

public class MasqActivity extends Activity {
    @Override
    public void onCreate(@Nullable Bundle savedInstanceState){
        super.onCreate(savedInstanceState);
        setContentView(R.layout.masq_layout);

        findViewById(R.id.button_masq_back).setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View view) {
                Intent backToLanding = new Intent(MasqActivity.this, MainActivity.class);
                startActivity(backToLanding);
            }
        });
    }
}
