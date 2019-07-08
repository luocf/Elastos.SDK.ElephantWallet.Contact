package org.elastos.sdk.contact.test;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import org.elastos.sdk.contact.BlockChainNode;
import org.elastos.sdk.contact.Did;
import org.elastos.sdk.contact.DidManager;
import org.elastos.sdk.contact.HDContact;
import org.elastos.sdk.contact.Identity;
import org.elastos.sdk.contact.IdentityManager;
import org.elastos.sdk.contact.Transaction;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;

import static org.elastos.sdk.contact.IdentityManager.getMnemonic;

public class MainActivity extends Activity {
    private static final String TAG = "MainActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

    }

    @Override
    protected void onStart() {
        super.onStart();

        TextView txtMsg = findViewById(R.id.txt_message);

        findViewById(R.id.btn_test_mnemonic).setOnClickListener((view) -> {
            String message = testGenrateMnemonic();
            txtMsg.setText(message);
        });
        findViewById(R.id.btn_test_singlecontact).setOnClickListener((view) -> {
            testSingleContact();
        });
        findViewById(R.id.btn_test_hdcontact).setOnClickListener((view) -> {
            testHDContactAddress();
        });
        findViewById(R.id.btn_test_did).setOnClickListener((view) -> {
            String message = testDid();
            txtMsg.setText(message);
        });
        findViewById(R.id.btn_test_sign).setOnClickListener((view) -> {
            String message = testSignTxData();
            message += testCosignTxData();
            txtMsg.setText(message);
        });
    }

    private String readWords() {
        StringBuilder buf = new StringBuilder();
        InputStream file = null;
        try {
            file = getAssets().open("mnemonic_chinese.txt");
            BufferedReader in = new BufferedReader(new InputStreamReader(file, "UTF-8"));

            String str;

            while ((str = in.readLine()) != null) {
                buf.append(str);
                buf.append('\n');
            }

            in.close();
        } catch (IOException e) {
            e.printStackTrace();
        }

//        Log.d("Test", buf.toString());
        return buf.toString();
    }

    private String testGenrateMnemonic() {
        return null;
    }

    private void testSingleContact() {
        Log.d("Test", "balance: " + balance);
    }

    private void testHDContactAddress() {
        Log.d("Test", "txid: " + txid);
    }

    private String testDid() {
        Log.d("Test", "name: " + value);
        return "";
    }

    private String testSignTxData() {
        String message = "";

        message += "================================================\n";
        return message;
    }

    private String testCosignTxData() {
        String message = "";

        message += "================================================\n";
        return message;
    }
}
