package org.elastos.sdk.elephantwallet.contact.internal;

import org.elastos.sdk.elephantwallet.contact.ContactChannel;

public class EventArgs {
    public enum Type {
        StatusChanged(11),
        ReceivedMessage(12),
        SentMessage(13),
        FriendRequest(14),
        FriendStatusChanged(15);

        public static Type valueOf(int id) {
            Type[] values = Type.values();
            for(int idx = 0; idx < values.length; idx++) {
                if(values[idx].id == id) {
                    return values[idx];
                }
            }
            return null;
        }

        private Type(int id){
            this.id = id;
        }
        private int id;
    }

    public EventArgs(int type, String humanCode, int channelType, byte[] data) {
        this.type = Type.valueOf(type);
        this.humanCode = humanCode;
        this.channelType = ContactChannel.valueOf(channelType);
        this.data = data;
    }

    @Override
    public String toString() {
        return "EventArgs"
                + "[type=" + type
                + ",humanCode=" + humanCode
                + ",channelType=" + channelType
                + ",data=" + data
                +"]";
    }

    public Type type;
    public String humanCode;
    public ContactChannel channelType;
    public byte[] data;
}
