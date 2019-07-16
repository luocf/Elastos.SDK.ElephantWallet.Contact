package org.elastos.sdk.elephantwallet.contact.internal;

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

    public ContactStatus status;

    public String chainPubKey;
    public String did;
    public String elaAddress;
    public String nickname;
    public String avatar;
    public String gender;
    public String description;
    @SerializedName(BoundCarrierArray)
    public List<CarrierInfo> boundCarrierArray;

    public int deserialize(String value, ContactStatus status) {
        Gson gson = new Gson();

        HumanInfo info = gson.fromJson(value, HumanInfo.class);
        info.chainPubKey = info.commonInfoMap.get(Item.ChainPubKey.id());
        this.did = info.commonInfoMap.get(Item.Did.id());
        this.elaAddress = info.commonInfoMap.get(Item.ElaAddress.id());
        this.nickname = info.commonInfoMap.get(Item.Nickname.id());
        this.avatar = info.commonInfoMap.get(Item.Avatar.id());
        this.gender = info.commonInfoMap.get(Item.Gender.id());
        this.description = info.commonInfoMap.get(Item.Description.id());
        this.commonInfoMap = null;
        this.boundCarrierArray = info.boundCarrierArray;

        this.status = status;

        return 0;
    }

    @Override
    public String toString() {
        Gson gson = new Gson();
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

    @SerializedName(CommonInfoMap)
    private HashMap<Integer, String> commonInfoMap;

    private static final String BoundCarrierArray = "BoundCarrierArray";
    private static final String CommonInfoMap = "CommonInfoMap";
} // class Factory
