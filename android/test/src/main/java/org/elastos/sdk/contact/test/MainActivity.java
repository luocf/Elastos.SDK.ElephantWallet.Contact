package org.elastos.sdk.contact.test;

import android.app.Activity;
import android.app.AlarmManager;
import android.app.AlertDialog;
import android.app.PendingIntent;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Process;
import android.preference.PreferenceManager;
import android.provider.Settings;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;
import android.widget.Toast;

import org.elastos.sdk.elephantwallet.contact.Contact;
import org.elastos.sdk.elephantwallet.contact.internal.ContactChannel;
import org.elastos.sdk.elephantwallet.contact.internal.ContactStatus;
import org.elastos.sdk.elephantwallet.contact.internal.EventArgs;
import org.elastos.sdk.elephantwallet.contact.internal.AcquireArgs;
import org.elastos.sdk.elephantwallet.contact.internal.Utils;
import org.elastos.sdk.keypair.ElastosKeypair;

import java.nio.charset.Charset;
import java.util.List;

public class MainActivity extends Activity {
    public static final String TAG = "ContactTest";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        TextView txtError = findViewById(R.id.txt_error);
        txtError.setMovementMethod(ScrollingMovementMethod.getInstance());
        TextView txtCbMsg = findViewById(R.id.txt_event);
        txtCbMsg.setMovementMethod(ScrollingMovementMethod.getInstance());
        TextView txtMsg = findViewById(R.id.txt_message);
        txtMsg.setMovementMethod(ScrollingMovementMethod.getInstance());
        txtMsg.setOnLongClickListener((v) -> {
            setClip(((TextView)v).getText().toString());
            return true;
        });

        String devId = getDeviceId();
        Log.i(TAG, "Device ID:" + devId);

        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(this);
        mSavedMnemonic = pref.getString(SavedMnemonicKey, null);
        if(mSavedMnemonic == null) {
            mSavedMnemonic = ElastosKeypair.generateMnemonic(KeypairLanguage, KeypairWords);
//            if (devId.startsWith("7134d")) {
//                mSavedMnemonic = UploadedMnemonic1;
//            }
            if (devId.startsWith("fa65a")) {
                mSavedMnemonic = UploadedMnemonic2;
            }

            newAndSaveMnemonic(mSavedMnemonic);
        }

        showMessage("Mnemonic:\n" + mSavedMnemonic);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        Process.killProcess(Process.myPid());
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        String message = "";

        int id = item.getItemId();
        switch (id) {
            case R.id.get_started:
                getStarted();
                break;

            case R.id.clear_event: {
                clearEvent();
                break;
            }

            case R.id.new_mnemonic:
                message = newAndSaveMnemonic(null);
                break;
//            case R.id.save_mnemonic: {
//                saveMnemonic(mSavedMnemonic);
//                message = ("Success to save mnemonic:\n" + mSavedMnemonic);
//                break;
//            }
            case R.id.import_mnemonic:
                message = importMnemonic();
                break;

            case R.id.new_contact:
                message = testNewContact();
                break;
            case R.id.start_contact:
                message = testStart();
                break;
            case R.id.del_contact:
                message = testDelContact();
                break;

            case R.id.user_info:
                message = showUserInfo();
                break;
            case R.id.sync_upload:
                message = testSyncUpload();
                break;
            case R.id.sync_download:
                message = testSyncDownload();
                break;

            case R.id.friend_info:
                message = listFriendInfo();
                break;
            case R.id.add_friend:
                message = scanUserInfo();
                break;
            case R.id.send_message:
                message = sendMessage();
                break;
        }
        showMessage(message);

        return super.onOptionsItemSelected(item);
    }

    private void clearEvent() {
        TextView txtCbMsg = findViewById(R.id.txt_event);
        txtCbMsg.setText("");
    }

    private void getStarted() {
        String help = "";
        help += "Step1: [New Contact]\n";
        help += "Step2: [Start Contact]\n";
        help += "Step3: [User Info] can show you info\n";
        help += "Step4: After online, [Add friend] can add friend\n";
        help += "Step5: After friend online, [Send Message] can send message\n";

        clearEvent();
        showEvent(help);
    }

    private String importMnemonic() {
        Helper.showImportMnemonic(this, (result) -> {
            if(isEnglishWords(result) == false) {
                showMessage(ErrorPrefix + "Only english mnemonic is supported.");
                return;
            }
            String privKey = getPrivateKey();
            if(privKey == null || privKey.isEmpty()) {
                showMessage(ErrorPrefix + "Bad mnemonic.");
                return;
            }

            String message = newAndSaveMnemonic(result);
            showMessage(message);
        });

        return "Success to show import mnemonic dialog.";
    }

    private String testNewContact() {
        Contact.Factory.SetLogLevel(7);

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
            public void onError(int errCode, String errStr, String ext) {
                String msg = errCode + ": " + errStr;
                msg += "\n" + ext;
                showError(msg);
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
                info.did, info.getCurrDevCarrierAddr(), info.getCurrDevCarrierId()
        };
        Helper.showAddress(this, humanCode, getDeviceId(), (result) -> {
            Helper.showDetails(MainActivity.this, info.toJson());
        });

        return info.toString();
    }

    private String scanUserInfo() {
        if (mContact == null) {
            return (ErrorPrefix + "Contact is null.");
        }

        Contact.UserInfo info = mContact.getUserInfo();
        if (mContact.getStatus(info.humanCode) != ContactStatus.Online) {
            return (ErrorPrefix + "Contact is not online.");
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

        return "";
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

                ContactStatus status = mContact.getStatus(friendCode);
                if(status != ContactStatus.Online) {
                    showMessage(ErrorPrefix + "Friend is not online.");
                    return;
                }

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
                showToast(msg);
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

    public void showError(String newErr) {
        Handler handler = new Handler(Looper.getMainLooper());
        handler.post(() -> {
            Log.e(TAG, newErr);

            TextView txtCbMsg = findViewById(R.id.txt_error);
            txtCbMsg.setText(newErr);
        });
    }

    public void showToast(String msg) {
        Handler handler = new Handler(Looper.getMainLooper());
        handler.post(() -> {
            if(mToast != null) {
                mToast.cancel();
            }
            mToast = Toast.makeText(this, msg, Toast.LENGTH_LONG);
            mToast.show();
        });
    }

    private void setClip(String data) {
        ClipboardManager cManager = (ClipboardManager) getSystemService(Context.CLIPBOARD_SERVICE);
        ClipData cData = ClipData.newPlainText("text", data);
        cManager.setPrimaryClip(cData);
        showToast("Copied.");
    }

    private String newAndSaveMnemonic(final String newMnemonic) {
        mSavedMnemonic = newMnemonic;
        if(mSavedMnemonic == null) {
            mSavedMnemonic = ElastosKeypair.generateMnemonic(KeypairLanguage, KeypairWords);
        }

        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(this);
        SharedPreferences.Editor editor = pref.edit();
        editor.putString(SavedMnemonicKey, mSavedMnemonic).commit();
        if(mContact == null) { // noneed to restart
            return ("Success to save mnemonic:\n" + mSavedMnemonic);
        }

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setMessage("New mnemonic can only be valid after restart,\ndo you want restart app?");
        builder.setPositiveButton("Restart", (dialog, which) -> {
            // restart
            Intent mStartActivity = new Intent(this, MainActivity.class);
            int mPendingIntentId = 123456;
            PendingIntent mPendingIntent = PendingIntent.getActivity(this, mPendingIntentId, mStartActivity, PendingIntent.FLAG_CANCEL_CURRENT);
            AlarmManager mgr = (AlarmManager)this.getSystemService(Context.ALARM_SERVICE);
            mgr.set(AlarmManager.RTC, System.currentTimeMillis() + 100, mPendingIntent);
            Process.killProcess(Process.myPid());
        });
        builder.setNegativeButton("Cancel", (dialog, which) -> {
            dialog.dismiss();
        });
        new Handler(Looper.getMainLooper()).post(() -> {
            builder.create().show();
        });

        return ("Cancel to save mnemonic:\n" + newMnemonic);
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

    private boolean isEnglishWords(String words) {
        boolean isEnglish = Charset.forName("US-ASCII").newEncoder().canEncode(words);
        return isEnglish;
    }

    String mSavedMnemonic;
    Contact mContact;
    Contact.Listener mContactListener;

    Thread mThread;
    Toast mToast;

    private static final String ErrorPrefix = "Error: ";

    private static final String KeypairLanguage = "english";
    private static final String KeypairWords = "";

    private static final String SavedMnemonicKey = "mnemonic";


    private static final String UploadedMnemonic1 = "ceiling detail diet cotton shed false under riot leaf wait escape busy";
    private static final String UploadedMnemonic2 = "museum girl useless quarter cage turtle stomach law govern muscle kiwi crack";
}
