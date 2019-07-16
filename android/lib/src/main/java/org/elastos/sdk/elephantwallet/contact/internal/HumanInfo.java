package org.elastos.sdk.elephantwallet.contact.internal;

import android.support.annotation.CallSuper;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.annotations.SerializedName;

import java.util.HashMap;
import java.util.List;

public class HumanInfo {
    public class CarrierInfo {
        public class DeviceInfo {
            @SerializedName("DevId")
            public String devId;
            @SerializedName("DevName")
            public String devName;
            @SerializedName("UpdateTime")
            public long updateTime;

            @Override
            public String toString() { return Utils.ToString(this); }
        }

        @SerializedName("CarrierAddr")
        public String usrAddr;
        @SerializedName("CarrierId")
        public String usrId;
        @SerializedName("DeviceInfo")
        public DeviceInfo devInfo;

        @Override
        public String toString() { return Utils.ToString(this); }
    }

    @SerializedName(BoundCarrierArray)
    public List<CarrierInfo> boundCarrierArray;
    @SerializedName(CommonInfoMap)
    private HashMap<Integer, String> commonInfoMap;
    @SerializedName(Status)
    public ContactStatus status;
    @SerializedName(HumanCode)
    public String humanCode;

    public String chainPubKey;
    public String did;
    public String elaAddress;
    public String nickname;
    public String avatar;
    public String gender;
    public String description;

    @CallSuper
    public int fromJson(String value) {
        HumanJson info = new Gson().fromJson(value, HumanJson.class);
        if(info.humanInfo == null) {
            return -1;
        }

        this.commonInfoMap = null;
        this.boundCarrierArray = info.humanInfo.boundCarrierArray;
        this.status = info.humanInfo.status;
        this.humanCode = info.humanInfo.humanCode;

        this.chainPubKey = info.humanInfo.commonInfoMap.get(Item.ChainPubKey.id());
        this.did = info.humanInfo.commonInfoMap.get(Item.Did.id());
        this.elaAddress = info.humanInfo.commonInfoMap.get(Item.ElaAddress.id());
        this.nickname = info.humanInfo.commonInfoMap.get(Item.Nickname.id());
        this.avatar = info.humanInfo.commonInfoMap.get(Item.Avatar.id());
        this.gender = info.humanInfo.commonInfoMap.get(Item.Gender.id());
        this.description = info.humanInfo.commonInfoMap.get(Item.Description.id());

        return 0;
    }

    public String toJson() {
        Gson gson = new GsonBuilder().setPrettyPrinting().create();
        String val = gson.toJson(this);
        return val;
    }

    public String prettyPrint() {
        Gson gson = new GsonBuilder().setPrettyPrinting().create();
        String val = gson.toJson(this);
        return val;
    }

    protected HumanInfo() {
    }

    private enum Item {
        ChainPubKey(1),
        Did(2),
        ElaAddress(3),
        Nickname(4),
        Avatar(5),
        Gender(6),
        Description(7);

        int id(){
            return this.id;
        }

        private Item(int id){
            this.id = id;
        }
        private int id;
    }

    private class HumanJson {
        @SerializedName("HumanInfo")
        HumanInfo humanInfo;
    }

    private static final String BoundCarrierArray = "BoundCarrierArray";
    private static final String CommonInfoMap = "CommonInfoMap";
    private static final String Status = "Status";
    private static final String HumanCode = "HumanCode";
} // class Factory
