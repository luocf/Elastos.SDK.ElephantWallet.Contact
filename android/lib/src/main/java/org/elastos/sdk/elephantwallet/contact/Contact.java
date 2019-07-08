package org.elastos.sdk.elephantwallet.contact;

public final class Contact {
    public static final class Factory {
        public static native void SetLogLevel(int level);
        public static native int SetLocalDataDir(String dir);
        public static native Contact Create();
    } // class Factory

}
