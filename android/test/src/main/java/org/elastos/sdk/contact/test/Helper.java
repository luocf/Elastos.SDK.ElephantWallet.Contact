package org.elastos.sdk.contact.test;

import android.app.AlertDialog;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;

import com.google.zxing.BarcodeFormat;
import com.google.zxing.EncodeHintType;
import com.google.zxing.MultiFormatWriter;
import com.google.zxing.WriterException;
import com.google.zxing.common.BitMatrix;
import com.google.zxing.qrcode.decoder.ErrorCorrectionLevel;

import org.elastos.sdk.elephantwallet.contact.Contact;

import java.util.HashMap;

public class Helper {
    public static void showAddress(Context context, String did, String carrier) {
        AlertDialog.Builder builder = new AlertDialog.Builder(context);
        try {
//            String address = CarrierHelper.getAddress();
//            Log.i(Logger.TAG, "show address: " + address);
            View root = makeAddressView(context, did, carrier);

            builder.setMessage("My Address");
            builder.setView(root);
        } catch (Exception e) {
            builder.setMessage("Failed to show address." + e);
        }

        builder.setPositiveButton("OK", (dialog, which) -> {
            dialog.dismiss();
        });
        builder.create().show();
    }

    private static View makeAddressView(Context context, String did, String carrier) {
        ImageView image = new ImageView(context);
        TextView txt = new TextView(context);

        RadioGroup radioGrp = new RadioGroup(context);
        RadioButton btnDid = new RadioButton(context);
        btnDid.setId(View.generateViewId());
        btnDid.setText("Did");
        RadioButton btnCarrier = new RadioButton(context);
        btnCarrier.setId(View.generateViewId());
        btnCarrier.setText("Carrier");
        radioGrp.addView(btnDid);
        radioGrp.addView(btnCarrier);

        LinearLayout root = new LinearLayout(context);
        root.setOrientation(LinearLayout.VERTICAL);
        root.addView(image);
        root.addView(txt);
        root.addView(radioGrp);

        ViewGroup.MarginLayoutParams txtLayout = (ViewGroup.MarginLayoutParams) txt.getLayoutParams();
        txtLayout.setMargins(100, 100, 100, 100);

        radioGrp.setOnCheckedChangeListener((group, checkedId) -> {
            String value = (checkedId == btnCarrier.getId() ? carrier : did);
            Bitmap bitmap = makeQRCode(value);
            image.setImageBitmap(bitmap);
            txt.setText(value);
        });
        radioGrp.check(btnDid.getId());

        return root;
    }

    private static Bitmap makeQRCode(String value) {
        HashMap<EncodeHintType, ErrorCorrectionLevel> hintMap = new HashMap<>();
        hintMap.put(EncodeHintType.ERROR_CORRECTION, ErrorCorrectionLevel.L);
        BitMatrix matrix = null;
        try {
            matrix = new MultiFormatWriter().encode(value, BarcodeFormat.QR_CODE, 512, 512, hintMap);
        } catch (WriterException e) {
            Log.e(MainActivity.TAG, "Failed to MultiFormatWriter().encode()", e);
            throw new RuntimeException("Failed to MultiFormatWriter().encode()", e);
        }

        //converting bitmatrix to bitmap
        int width = matrix.getWidth();
        int height = matrix.getHeight();
        int[] pixels = new int[width * height];
        // All are 0, or black, by default
        for (int y = 0; y < height; y++) {
            int offset = y * width;
            for (int x = 0; x < width; x++) {
                pixels[offset + x] = matrix.get(x, y) ? Color.BLACK : Color.WHITE;
            }
        }

        Bitmap bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        bitmap.setPixels(pixels, 0, width, 0, 0, width, height);

        return bitmap;
    }
}
