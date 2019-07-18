package org.elastos.sdk.elephantwallet.contact.internal;

import org.elastos.tools.crosspl.CrossBase;
import org.elastos.tools.crosspl.annotation.CrossClass;
import org.elastos.tools.crosspl.annotation.CrossInterface;

@CrossClass
public class ContactMessage extends CrossBase {
    public enum Type {
        Empty(0x00000000),
        MsgText(0x00000001),
        MsgAudio(0x00000002),
        MsgTransfer(0x00000004);

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

    public final Type type;
    public final byte[] data;
    public final String cryptoAlgorithm;
    public long timestamp;

    public int syncMessageToNative() {
        int ret = syncMessageToNative(type.id, data, cryptoAlgorithm, timestamp);
        return ret;
    }

    protected ContactMessage(Type type, byte[] data, String cryptoAlgorithm) {
        super(ContactMessage.class.getName(), 0);

        this.type = type;
        this.data = data;
        this.cryptoAlgorithm = cryptoAlgorithm;
        this.timestamp = System.currentTimeMillis();
    }

    @CrossInterface
    private native int syncMessageToNative(int type, byte[] data, String cryptoAlgorithm, long timestamp);
} // class Factory
