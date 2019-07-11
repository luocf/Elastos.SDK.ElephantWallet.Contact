package org.elastos.sdk.contact.test;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Process;
import android.provider.Settings;
import android.util.Log;
import android.widget.TextView;

import org.elastos.sdk.elephantwallet.contact.Contact;
import org.elastos.sdk.elephantwallet.contact.internal.RequestArgs;
import org.elastos.sdk.elephantwallet.contact.internal.Utils;
import org.elastos.sdk.keypair.ElastosKeypair;

import java.security.KeyPair;

public class MainActivity extends Activity {
    private static final String TAG = "ContactTest";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mSavedMnemonic = "bachelor sail glove swing despair lawsuit exhibit travel slot practice latin glass";
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

        String devId = Settings.Secure.getString(getContentResolver(), Settings.Secure.ANDROID_ID);
        Contact.Factory.SetDeviceId(devId);

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

//        mThread = new Thread(() -> {
            int ret = mContact.start();
//        });
//        mThread.start();
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
                response = getPublicKey().getBytes();
                break;
            case EncryptData:
                response = request.data; // plaintext
                break;
            case DecryptData:
                response = request.data; // plaintext
                break;
            case DidPropAppId:
                String appId = "DC92DEC59082610D1D4698F42965381EBBC4EF7DBDA08E4B3894D530608A64AAA65BB82A170FBE16F04B2AF7B25D88350F86F58A7C1F55CC29993B4C4C29E405";
                response = appId.getBytes();
                break;
            case DidAgentAuthHeader:
                response = getAgentAuthHeader();
                break;
            case SignData:
                response = signData(request.data);
                break;
        }


        return response;
    }

    private String getPublicKey() {
        ElastosKeypair.Data seedData = new ElastosKeypair.Data();
        int seedSize = ElastosKeypair.getSeedFromMnemonic(seedData, mSavedMnemonic,
                                                          KeypairLanguage, KeypairWords, "");
        String pubKey = ElastosKeypair.getSinglePublicKey(seedData, seedSize);
        return pubKey;
    }

    private byte[] getAgentAuthHeader() {
        String appid = "org.elastos.debug.didplugin";
        String appkey = "b2gvzUM79yLhCbbGNWCuhSsGdqYhA7sS";
        long timestamp = System.currentTimeMillis();
        String auth = Utils.getMd5(appkey + timestamp);
        String headerValue = "id=" + appid + ";time=" + timestamp + ";auth=" + auth;
        Log.i(TAG, "getAgentAuthHeader() headerValue=" + headerValue);

        return headerValue.getBytes();
    }

    private byte[] signData(byte[] data) {
        String pubKey = getPublicKey();

        ElastosKeypair.Data originData = new ElastosKeypair.Data();
        originData.buf = data;

        ElastosKeypair.Data signedData = new ElastosKeypair.Data();

        int signedSize = ElastosKeypair.sign(pubKey, originData, originData.buf.length, signedData);
        if(signedSize <= 0) {
            return null;
        }

        return signedData.buf;
    }

    private void appendCbMessage(TextView txtCbMsg, String msg) {
        Handler handler = new Handler(Looper.getMainLooper());
        handler.post(() -> {
            txtCbMsg.setText(msg);
        });
    }

    String mSavedMnemonic;
    Contact mContact;
    Contact.Listener mContactListener;

    Thread mThread;

    private static final String KeypairLanguage = "english";
    private static final String KeypairWords = "";
}
