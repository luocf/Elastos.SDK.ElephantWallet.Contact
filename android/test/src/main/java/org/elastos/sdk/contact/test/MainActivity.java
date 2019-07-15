package org.elastos.sdk.contact.test;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Process;
import android.provider.Settings;
import android.support.annotation.RequiresPermission;
import android.util.Log;
import android.widget.TextView;

import com.blikoon.qrcodescanner.QrCodeActivity;

import org.elastos.sdk.elephantwallet.contact.Contact;
import org.elastos.sdk.elephantwallet.contact.internal.EventArgs;
import org.elastos.sdk.elephantwallet.contact.internal.AcquireArgs;
import org.elastos.sdk.elephantwallet.contact.internal.HumanInfo;
import org.elastos.sdk.elephantwallet.contact.internal.Utils;
import org.elastos.sdk.keypair.ElastosKeypair;
import org.w3c.dom.Text;

import java.lang.reflect.Method;
import java.security.KeyPair;

public class MainActivity extends Activity {
    public static final String TAG = "ContactTest";

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

        findViewById(R.id.btn_test_newcontact).setOnClickListener((view) -> {
            String message = testNewContact();
            showMessage(message);
        });
        findViewById(R.id.btn_test_start).setOnClickListener((view) -> {
            String message = testStart();
            showMessage(message);
        });

        findViewById(R.id.btn_show_userinfo).setOnClickListener((view) -> {
            String message = showUserInfo();
            showMessage(message);
        });

        findViewById(R.id.btn_scan_userinfo).setOnClickListener((view) -> {
            scanUserInfo();
        });

        findViewById(R.id.btn_test_delcontact).setOnClickListener((view) -> {
            String message = testDelContact();
            showMessage(message);
        });
        findViewById(R.id.btn_test_dellistener).setOnClickListener((view) -> {
            String message = testDelListener();
            showMessage(message);
        });
    }


    private String testNewContact() {
        Contact.Factory.SetLogLevel(4);

        Contact.Factory.SetDeviceId(getDeviceId());

        int ret = Contact.Factory.SetLocalDataDir(this.getCacheDir().getAbsolutePath());
        if(ret < 0) {
            return "Failed to call Contact.Factory.SetLocalDataDir() ret=" + ret;
        }

        mContact = Contact.Factory.Create();
        if(mContact == null) {
            return "Failed to call Contact.Factory.Create()";
        }

        if(mContactListener != null) {
            mContactListener = null;
        }
        mContactListener = new Contact.Listener() {
            @Override
            public byte[] onAcquire(AcquireArgs request) {
                byte[] ret = processAcquire(request);

                String msg = "onAcquire(): req=" + request + "\n";
                msg += "onAcquire(): resp=" + ret + "\n";
                showEvent(msg);

                return ret;
            }

            @Override
            public void onEvent(EventArgs event) {
                processEvent(event);

                String msg = "onEvent(): ev=" + event + "\n";
                showEvent(msg);
            }

            @Override
            public void onError(int errCode, String errStr) {
                String msg = "onError";
                msg += " errCode=" + errCode;
                msg += " errStr=" + errStr;
                showEvent(msg);
            }
        };
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

    private String showUserInfo() {
        if (mContact == null) {
            return "Contact is null.";
        }

        Contact.UserInfo info = mContact.getUserInfo();

        Helper.showAddress(this, info.did, info.getCurrDevCarrierAddr());

        return info.toString();
    }

    private void scanUserInfo() {
        if (mContact == null) {
            showMessage("Contact is null.");
            return;
        }

        Helper.scanAddress(this, result -> {
            showMessage(result);

            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle("Find Address");
            builder.setMessage(result);
            builder.setNegativeButton("Cancel", (dialog, which) -> {
                dialog.dismiss();
            });
            builder.setPositiveButton("Add Friend", (dialog, which) -> {
                mContact.addFriend(result, "Hello");
            });
            builder.create().show();

        });

        return;
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

    private byte[] processAcquire(AcquireArgs request) {
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
            default:
                throw new RuntimeException("Unprocessed request: " + request);
        }


        return response;
    }

    private void processEvent(EventArgs event) {
        switch (event.type) {
            case StatusChanged:
                break;
            case ReceivedMessage:
                break;
            case SentMessage:
                break;
            case FriendRequest:
                break;
            case FriendStatusChanged:
                break;
            default:
                Log.w(TAG, "Unprocessed event: " + event);
        }
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

    private String getDeviceId() {
        String devId = Settings.Secure.getString(getContentResolver(), Settings.Secure.ANDROID_ID);
        return devId;
    }

    public void showMessage(String msg) {
        Handler handler = new Handler(Looper.getMainLooper());
        handler.post(() -> {
            Log.i(TAG, msg);
            TextView txtMsg = findViewById(R.id.txt_message);
            txtMsg.setText(msg);
        });
    }

    public void showEvent(String newMsg) {
        Handler handler = new Handler(Looper.getMainLooper());
        handler.post(() -> {
            TextView txtCbMsg = findViewById(R.id.txt_event);
            String msg = txtCbMsg.getText().toString();
            msg += "\n";
            msg += newMsg;

            Log.i(TAG, msg);
            txtCbMsg.setText(msg);
        });
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        Helper.onRequestPermissionsResult(this, requestCode, permissions, grantResults);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        Helper.onActivityResult(this, requestCode, resultCode, data);
    }

    String mSavedMnemonic;
    Contact mContact;
    Contact.Listener mContactListener;

    Thread mThread;

    private static final String KeypairLanguage = "english";
    private static final String KeypairWords = "";
}
