package org.elastos.sdk.elephantwallet.contact.internal;

public class RequestArgs {
    public enum Type {
        PublicKey(21),
        EncryptData(22),
        DecryptData(23),
        DidPropAppId(24),
        DidAgentAuthHeader(25),
        SignData(26);

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

    public RequestArgs(int type, String pubKey, byte[] data) {
        this.type = Type.valueOf(type);
        this.publicKey = pubKey;
        this.data = data;
    }

    @Override
    public String toString() {
        return "RequestArgs"
             + "[type=" + type
             + ",publicKey=" + publicKey
             + ",data=" + data
             +"]";
    }

    public Type type;
    public String publicKey;
    public byte[] data;
}
