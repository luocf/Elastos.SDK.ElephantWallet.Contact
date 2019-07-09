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
            String message = testNewContact();
            txtMsg.setText(message);
        });
        findViewById(R.id.btn_test_start).setOnClickListener((view) -> {
            String message = testStart();
            txtMsg.setText(message);
        });

        findViewById(R.id.btn_test_delcontact).setOnClickListener((view) -> {
            String message = testDelContact();
            txtMsg.setText(message);
        });
    }


    private String testPreset() {
        Contact.Factory.SetLogLevel(4);

        int ret = Contact.Factory.SetLocalDataDir(this.getCacheDir().getAbsolutePath());
        if(ret < 0) {
            return "Failed to call Contact.Factory.SetLocalDataDir() ret=" + ret;
        }

        return "Success to preset factory.";
    }

    private String testNewContact() {
        mContact = Contact.Factory.Create();
        if(mContact == null) {
            return "Failed to call Contact.Factory.Create()";
        }

        Contact.Listener listener = new Contact.Listener() {
            @Override
            public void onRequest(Request request) {

            }

            @Override
            public void onEvent(Event event) {

            }

            @Override
            public void onError(int errCode, String errStr) {

            }
        };
        mContact.setListener(listener);

        return "Success to create a contact instance.";
    }

    private String testStart() {
        return null;
    }

    private String testDelContact() {
        if(mContact == null) {
            return "Contact is null.";
        }

        mContact = null;
        System.gc();
        return "Success to delete a contact instance.";
    }

    Contact mContact;
}
