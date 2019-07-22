package org.elastos.sdk.contact.test;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Process;
import android.provider.Settings;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.widget.TextView;
import android.widget.Toast;

import org.elastos.sdk.elephantwallet.contact.Contact;
import org.elastos.sdk.elephantwallet.contact.internal.ContactChannel;
import org.elastos.sdk.elephantwallet.contact.internal.ContactStatus;
import org.elastos.sdk.elephantwallet.contact.internal.EventArgs;
import org.elastos.sdk.elephantwallet.contact.internal.AcquireArgs;
import org.elastos.sdk.elephantwallet.contact.internal.Utils;
import org.elastos.sdk.keypair.ElastosKeypair;
import java.util.List;

public class MainActivity extends Activity {
    public static final String TAG = "ContactTest";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        TextView txtMsg = findViewById(R.id.txt_message);
        txtMsg.setMovementMethod(ScrollingMovementMethod.getInstance());
        TextView txtCbMsg = findViewById(R.id.txt_event);
        txtCbMsg.setMovementMethod(ScrollingMovementMethod.getInstance());

        String devId = getDeviceId();
        if(devId.startsWith("fa65a")) {
//            mSavedMnemonic = ElastosKeypair.generateMnemonic(KeypairLanguage, KeypairWords);
            mSavedMnemonic = mUploadedMnemonic1;
        } else {
            mSavedMnemonic = ElastosKeypair.generateMnemonic(KeypairLanguage, KeypairWords);
//            mSavedMnemonic = mUploadedMnemonic2;
        }
        Log.i(TAG, "Device ID:" + devId);
        Log.i(TAG, "Mnemonic:" + mSavedMnemonic);
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
        findViewById(R.id.btn_test_delcontact).setOnClickListener((view) -> {
            String message = testDelContact();
            showMessage(message);
        });

        findViewById(R.id.btn_show_userinfo).setOnClickListener((view) -> {
            String message = showUserInfo();
            showMessage(message);
        });
        findViewById(R.id.btn_list_friendinfo).setOnClickListener((view) -> {
            listFriendInfo();
        });
        findViewById(R.id.btn_scan_userinfo).setOnClickListener((view) -> {
            scanUserInfo();
        });
        findViewById(R.id.btn_send_message).setOnClickListener((view) -> {
            String message = sendMessage();
            showMessage(message);
        });

        findViewById(R.id.btn_test_syncupload).setOnClickListener((view) -> {
            String message = testSyncUpload();
            showMessage(message);
        });
        findViewById(R.id.btn_test_syncdownload).setOnClickListener((view) -> {
            String message = testSyncDownload();
            showMessage(message);
        });


        findViewById(R.id.btn_clear_event).setOnClickListener((view) -> {
            TextView txtCbMsg = findViewById(R.id.txt_event);
            txtCbMsg.setText("");
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
            public void onReceivedMessage(String humanCode, int channelType, Contact.Message message) {
                Object data = message.data;
                if(message.type == Contact.Message.Type.MsgText) {
                    data = new String(message.data);
                }

                String msg = "onRcvdMsg(): data=" + data + "\n";
                msg += "onRcvdMsg(): type=" + message.type + "\n";
                msg += "onRcvdMsg(): crypto=" + message.cryptoAlgorithm + "\n";
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
            return ErrorPrefix + "Contact is null.";
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
            return ErrorPrefix + "Contact is null.";
        }

        mContact = null;
        System.gc(); // to test memory release
        return "Success to delete a contact instance.";
    }

    private String showUserInfo() {
        if (mContact == null) {
            return ErrorPrefix + "Contact is null.";
        }

        Contact.UserInfo info = mContact.getUserInfo();

        String[] humanCode = {
                info.did, info.getCurrDevCarrierAddr()
        };
        Helper.showAddress(this, humanCode, getDeviceId(), (result) -> {
            Helper.showDetails(MainActivity.this, info.toJson());
        });

        return info.toString();
    }

    private void scanUserInfo() {
        if (mContact == null) {
            showMessage(ErrorPrefix + "Contact is null.");
            return;
        }

        Contact.UserInfo info = mContact.getUserInfo();
        if (mContact.getStatus(info.humanCode) != ContactStatus.Online) {
            showMessage(ErrorPrefix + "Contact is not online.");
            return;
        }

        Helper.scanAddress(this, result -> {
            showMessage(result);

            Helper.showAddFriend(this, result, (summary) -> {
                int ret = mContact.addFriend(result, summary);
                if(ret < 0) {
                    showMessage(ErrorPrefix + "Failed to add friend. ret=" + ret);
                }
            });
        });

        return;
    }

    private String listFriendInfo() {
        if(mContact == null) {
            return ErrorPrefix + "Contact is null.";
        }

        List<Contact.FriendInfo> friendList = mContact.listFriendInfo();
        if(friendList == null) {
            return ErrorPrefix + "Failed to list friend info.";
        }
        List<String> friendCodeList = mContact.listFriendCode();

        Helper.showFriendList(this, friendCodeList, (friendCode) -> {
            Contact.FriendInfo friendInfo = null;
            for(Contact.FriendInfo info: friendList) {
                if(info.humanCode.equals(friendCode) == true) {
                    friendInfo = info;
                    break;
                }
            }
            Helper.showDetails(MainActivity.this, friendInfo.toJson());
        });
        return "Success to list friend info.";
    }

    private String sendMessage() {
        if (mContact == null) {
            return ErrorPrefix + "Contact is null.";
        }
        Contact.UserInfo info = mContact.getUserInfo();
        if (info.status != ContactStatus.Online) {
            return ErrorPrefix + "Contact is not online.";
        }

        List<String> friendCodeList = mContact.listFriendCode();
        Helper.showFriendList(this, friendCodeList, (friendCode) -> {
            Helper.showSendMessage(this, friendCode, (message) -> {
                Contact.Message msgInfo = mContact.makeTextMessage(message, null);

                int ret = mContact.sendMessage(friendCode, ContactChannel.Carrier, msgInfo);
                if(ret < 0) {
                    showMessage(ErrorPrefix + "Failed to send message to " + friendCode);
                }
            });
        });
        return "Success to send message.";
    }

    private String testSyncUpload() {
        if(mContact == null) {
            return ErrorPrefix + "Contact is null.";
        }

        int ret = mContact.syncInfoUploadToDidChain();
        if(ret < 0) {
            return ErrorPrefix + "Failed to call syncInfoUploadToDidChain() ret=" + ret;
        }

        return "Success to syncInfoUploadToDidChain.";
    }

    private String testSyncDownload() {
        if(mContact == null) {
            return ErrorPrefix + "Contact is null.";
        }

        int ret = mContact.syncInfoDownloadFromDidChain();
        if(ret < 0) {
            return ErrorPrefix + "Failed to call syncInfoDownloadToDidChain() ret=" + ret;
        }

        return "Success to syncInfoDownloadToDidChain.";
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
                Contact.Listener.RequestEvent requestEvent = (Contact.Listener.RequestEvent) event;
                Helper.showFriendRequest(this, requestEvent.humanCode, requestEvent.summary, v -> {
                    mContact.acceptFriend(requestEvent.humanCode);
                });
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

    private String getPrivateKey() {
        ElastosKeypair.Data seedData = new ElastosKeypair.Data();
        int seedSize = ElastosKeypair.getSeedFromMnemonic(seedData, mSavedMnemonic,
                KeypairLanguage, KeypairWords, "");
        String privKey = ElastosKeypair.getSinglePrivateKey(seedData, seedSize);
        return privKey;
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
        String privKey = getPrivateKey();

        ElastosKeypair.Data originData = new ElastosKeypair.Data();
        originData.buf = data;

        ElastosKeypair.Data signedData = new ElastosKeypair.Data();

        int signedSize = ElastosKeypair.sign(privKey, originData, originData.buf.length, signedData);
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

            if(msg.startsWith(ErrorPrefix)) {
                if(mToast != null) {
                    mToast.cancel();
                }
                mToast = Toast.makeText(this, msg, Toast.LENGTH_LONG);
                mToast.show();
            }
        });
    }

    public void showEvent(String newMsg) {
        Handler handler = new Handler(Looper.getMainLooper());
        handler.post(() -> {
            Log.i(TAG, newMsg);

            TextView txtCbMsg = findViewById(R.id.txt_event);
            String msg = txtCbMsg.getText().toString();
            msg += "\n";
            msg += newMsg;
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
    Toast mToast;

    private static final String ErrorPrefix = "Error: ";

    private static final String KeypairLanguage = "english";
    private static final String KeypairWords = "";

    private static final String mUploadedMnemonic1 = "ceiling detail diet cotton shed false under riot leaf wait escape busy";
    private static final String mUploadedMnemonic2 = "grit immune viable world merge inner picnic young twelve inject rather spoil";
}
