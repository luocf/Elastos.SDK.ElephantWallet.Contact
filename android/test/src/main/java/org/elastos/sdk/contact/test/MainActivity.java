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
        TextView txtCallbackMsg = findViewById(R.id.txt_callback);

        findViewById(R.id.btn_test_preset).setOnClickListener((view) -> {
            String message = testPreset();
            txtMsg.setText(message);
        });
        findViewById(R.id.btn_test_newcontact).setOnClickListener((view) -> {
            String message = testNewContact(txtCallbackMsg);
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
        findViewById(R.id.btn_test_dellistener).setOnClickListener((view) -> {
            String message = testDelListener();
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

    private String testNewContact(TextView txtCbMsg) {
        mContact = Contact.Factory.Create();
        if(mContact == null) {
            return "Failed to call Contact.Factory.Create()";
        }

        if(mContactListener != null) {
            mContactListener.unbind(); // to release native ref
            mContactListener = null;
        }
        mContactListener = new Contact.Listener() {
            @Override
            public void onRequest(Request request) {
                String msg = txtCbMsg.getText().toString();
                msg += "\n";
                msg += request;
                txtCbMsg.setText(msg);
            }

            @Override
            public void onEvent(Event event) {
                String msg = txtCbMsg.getText().toString();
                msg += "\n";
                msg += event;
                txtCbMsg.setText(msg);
            }

            @Override
            public void onError(int errCode, String errStr) {
                String msg = txtCbMsg.getText().toString();
                msg += "\nonError";
                msg += " errCode=" + errCode;
                msg += " errStr=" + errStr;
                txtCbMsg.setText(msg);
            }
        };
        mContactListener.bind(); // MUST call listener.unbind() by manual to release it
        mContact.setListener(mContactListener);

        return "Success to create a contact instance.";
    }

    private String testStart() {
        if(mContact == null) {
            return "Contact is null.";
        }

        int ret = mContact.start();
        if(ret < 0) {
            return "Failed to start contact instance. ret=" + ret;
        }

        return "Success to start contact instance.";
    }

    private String testDelContact() {
        if(mContact == null) {
            return "Contact is null.";
        }

        mContact = null;
        System.gc(); // to test memory release
        return "Success to delete a contact instance.";
    }

    private String testDelListener() {
        if(mContactListener == null) {
            return "Contact is null.";
        }

        mContactListener.unbind(); // to release native ref
        mContactListener = null;
        System.gc(); // to test memory release
        return "Success to delete a contact listener instance.";
    }

    Contact mContact;
    Contact.Listener mContactListener;
}
