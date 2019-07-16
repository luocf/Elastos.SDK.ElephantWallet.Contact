package org.elastos.sdk.elephantwallet.contact.internal;

import com.google.gson.Gson;

import java.util.Map;

public class FriendInfo extends HumanInfo {
    @Override
    public int fromJson(String value) {
        int ret = super.fromJson(value);
        if(ret < 0) {
            return ret;
        }

        return 0;
    }

    private static String mCurrDevId;
} // class FriendInfo

