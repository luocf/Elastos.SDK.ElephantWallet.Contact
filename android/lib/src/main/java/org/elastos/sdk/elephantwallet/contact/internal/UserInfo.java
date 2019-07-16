package org.elastos.sdk.elephantwallet.contact.internal;

import com.google.gson.Gson;

import java.util.Map;

public class UserInfo extends HumanInfo {
    public static void setCurrDevId(String devId) {
        mCurrDevId = devId;
    }

    public String getCurrDevCarrierAddr() {
        for(CarrierInfo info: boundCarrierArray) {
            if(info.devInfo.devId.equals(mCurrDevId) == true) {
                return info.usrAddr;
            }
        }
        return null;
    }

    @Override
    public int fromJson(String value) {
        int ret = super.fromJson(value);
        if(ret < 0) {
            return ret;
        }

        return 0;
    }

    private static String mCurrDevId;
} // class UserInfo

