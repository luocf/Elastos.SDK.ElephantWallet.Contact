package org.elastos.sdk.contact.test;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;

import org.elastos.sdk.elephantwallet.contact.Contact;

public class MainActivity extends Activity {
    private static final String TAG = "ContactTest";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

    }

    @Override
    protected void onStart() {
        super.onStart();

        TextView txtMsg = findViewById(R.id.txt_message);

        findViewById(R.id.btn_test_preset).setOnClickListener((view) -> {
            String message = testPreset();
            txtMsg.setText(message);
        });
        findViewById(R.id.btn_test_newcontact).setOnClickListener((view) -> {
            testNewContact();
        });
        findViewById(R.id.btn_test_start).setOnClickListener((view) -> {
            testStart();
        });
    }


    private String testPreset() {
        Contact.Factory.SetLogLevel(4);

        int ret = Contact.Factory.SetLocalDataDir(this.getCacheDir().getAbsolutePath());
        if(ret < 0) {
            throw new RuntimeException("Failed to call Contact.Factory.SetLocalDataDir() ret=" + ret);
        }

        return null;
    }

    private String testNewContact() {
        return null;
    }

    private String testStart() {
        return null;
    }

}
