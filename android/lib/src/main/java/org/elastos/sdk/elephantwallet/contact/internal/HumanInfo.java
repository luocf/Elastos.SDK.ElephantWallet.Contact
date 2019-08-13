package org.elastos.sdk.elephantwallet.contact.internal;

import android.support.annotation.CallSuper;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.annotations.SerializedName;

import java.util.HashMap;
import java.util.List;

public class HumanInfo {
    public enum Item {
        ChainPubKey(1),
        Did(2),
        ElaAddress(3),
        Nickname(4),
        Avatar(5),
        Gender(6),
        Description(7);

        public static Item valueOf(int id) {
            Item[] values = Item.values();
            for(int idx = 0; idx < values.length; idx++) {
                if(values[idx].id == id) {
                    return values[idx];
                }
            }

            return null;
        }

        int id(){
            return this.id;
        }

        private Item(int id){
            this.id = id;
        }
        private int id;
    }

    public class HumanJson {
        @SerializedName(JsonKey.HumanInfo)
        HumanInfo humanInfo;
    }

    public class CarrierInfo {
        public class DeviceInfo {
            @SerializedName(JsonKey.DeviceId)
            public String devId;
            @SerializedName(JsonKey.DeviceName)
            public String devName;
            @SerializedName(JsonKey.UpdateTime)
            public long updateTime;

            @Override
            public String toString() { return Utils.ToString(this); }
        }

        @SerializedName(JsonKey.CarrierAddr)
        public String usrAddr;
        @SerializedName(JsonKey.CarrierId)
        public String usrId;
        @SerializedName(JsonKey.DeviceInfo)
        public DeviceInfo devInfo;

        @Override
        public String toString() { return Utils.ToString(this); }
    }

    @SerializedName(JsonKey.BoundCarrierArray)
    public List<CarrierInfo> boundCarrierArray;
    @SerializedName(JsonKey.CommonInfoMap)
    private HashMap<Integer, String> commonInfoMap;
    @SerializedName(JsonKey.Status)
    public ContactStatus status;
    @SerializedName(JsonKey.HumanCode)
    public String humanCode;
    @SerializedName(JsonKey.UpdateTime)
    public long updateTime;

    public String chainPubKey;
    public String did;
    public String elaAddress;
    public String nickname;
    public String avatar;
    public String gender;
    public String description;

//    @CallSuper
//    public int fromJson(String value) {
//        HumanJson info = new Gson().fromJson(value, HumanJson.class);
//        if (info.humanInfo == null) {
//            return -1;
//        }
//
//        int ret = fromJson(info);
//
//        return ret;
//    }

    @CallSuper
    public int fromJson(HumanJson info) {
        this.commonInfoMap = null;
        this.boundCarrierArray = info.humanInfo.boundCarrierArray;
        this.status = info.humanInfo.status;
        this.humanCode = info.humanInfo.humanCode;
        this.updateTime = info.humanInfo.updateTime;

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
} // class Factory
