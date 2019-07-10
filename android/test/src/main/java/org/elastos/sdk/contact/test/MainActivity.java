package org.elastos.sdk.contact.test;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Process;
import android.widget.TextView;

import org.elastos.sdk.elephantwallet.contact.Contact;
import org.elastos.sdk.elephantwallet.contact.internal.RequestArgs;

public class MainActivity extends Activity {
    private static final String TAG = "ContactTest";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        Process.killProcess(Process.myPid());
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
            public byte[] onRequest(RequestArgs request) {
                byte[] ret = processRequest(request);

                String msg = txtCbMsg.getText().toString();
                msg += "\n";
                msg += "onRequest(): req=" + request + "\n";
                msg += "onRequest(): resp=" + ret + "\n";
                appendCbMessage(txtCbMsg, msg);

                return ret;
            }

            @Override
            public void onEvent(EventArgs event) {
                String msg = txtCbMsg.getText().toString();
                msg += "\n";
                msg += event;
                appendCbMessage(txtCbMsg, msg);
            }

            @Override
            public void onError(int errCode, String errStr) {
                String msg = txtCbMsg.getText().toString();
                msg += "\nonError";
                msg += " errCode=" + errCode;
                msg += " errStr=" + errStr;
                appendCbMessage(txtCbMsg, msg);
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

        mThread = new Thread(() -> {
            int ret = mContact.start();
        });
        mThread.start();
//        if(ret < 0) {
//            return "Failed to start contact instance. ret=" + ret;
//        }

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

    private byte[] processRequest(RequestArgs request) {
        byte[] response = null;

        switch (request.type) {
            case PublicKey:
                response = mSecret.publicKey.getBytes();
                break;
            case EncryptData:
                break;
            case DecryptData:
                break;
            case DidPropAppId:
                String appId = "DC92DEC59082610D1D4698F42965381EBBC4EF7DBDA08E4B3894D530608A64AAA65BB82A170FBE16F04B2AF7B25D88350F86F58A7C1F55CC29993B4C4C29E405";
                response = appId.getBytes();
                break;
            case DidAgentAuthHeader:
                break;
            case SignData:
                break;
        }


        return response;
    }

    private void appendCbMessage(TextView txtCbMsg, String msg) {
        Handler handler = new Handler(Looper.getMainLooper());
        handler.post(() -> {
            txtCbMsg.setText(msg);
        });
    }

    class Secret {
        Secret(String pubKey, String privKey) {
           this.publicKey = pubKey;
           this.privateKey = privKey;
        }

        final String publicKey;
        final String privateKey;
    }

    Secret mSecret = new Secret(
            "02bc11aa5c35acda6f6f219b94742dd9a93c1d11c579f98f7e3da05ad910a48306",
            "543c241f89bebb660157bcd12d7ab67cf69f3158240a808b22eb98447bad205d"
    );
    Contact mContact;
    Contact.Listener mContactListener;

    Thread mThread;
}
