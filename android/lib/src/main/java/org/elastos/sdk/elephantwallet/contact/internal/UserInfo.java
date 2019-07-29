package org.elastos.sdk.elephantwallet.contact.internal;

public class UserInfo extends HumanInfo {
    public class UserJson extends HumanJson {
    }

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

    public String getCurrDevCarrierId() {
        for(CarrierInfo info: boundCarrierArray) {
            if(info.devInfo.devId.equals(mCurrDevId) == true) {
                return info.usrId;
            }
        }
        return null;
    }

    @Override
    public int fromJson(HumanJson info) {
        if(info instanceof UserJson == false) {
            return -1;
        }

        int ret = super.fromJson(info);
        if(ret < 0) {
            return ret;
        }

        return 0;
    }

    private static String mCurrDevId;
} // class UserInfo

