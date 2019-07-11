package org.elastos.sdk.elephantwallet.contact.internal;

import java.math.BigInteger;
import java.security.MessageDigest;

public final class Utils {
    private Utils() { }

    public static String getMd5(String str) {
        try {
            MessageDigest md = MessageDigest.getInstance("MD5");
            md.update(str.getBytes());
            String md5 = new BigInteger(1, md.digest()).toString(16);
            return fillMD5(md5);
        } catch (Exception e) {
            throw new RuntimeException("Failed to get md5.", e);
        }
    }

    private static String fillMD5(String md5) {
        return md5.length() == 32 ? md5 : fillMD5("0" + md5);
    }
}

