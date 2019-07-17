package org.elastos.sdk.elephantwallet.contact.internal;

import com.google.gson.annotations.SerializedName;

public class FriendInfo extends HumanInfo {
    public class FriendJson extends HumanJson {
        @SerializedName("Alias")
        String alias;
    }

    public String alias;

    @Override
    public int fromJson(HumanJson info) {
        if(info instanceof FriendJson == false) {
            return -1;
        }

        int ret = super.fromJson(info);
        if(ret < 0) {
            return ret;
        }

        this.alias = ((FriendJson) info).alias;

        return 0;
    }
} // class FriendInfo

