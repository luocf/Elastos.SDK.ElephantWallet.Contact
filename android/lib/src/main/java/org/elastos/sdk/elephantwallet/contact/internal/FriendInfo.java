package org.elastos.sdk.elephantwallet.contact.internal;

import com.google.gson.Gson;

import java.util.Map;

public class FriendInfo extends HumanInfo {
    @Override
    public int deserialize(String value, ContactStatus status) {
        Gson gson = new Gson();
        Map<String, String> info = gson.fromJson(value, Map.class);

        int ret = super.deserialize(info.get("HumanInfo"), status);
        if(ret < 0) {
            return ret;
        }

        return 0;
    }

    private static String mCurrDevId;
} // class FriendInfo

