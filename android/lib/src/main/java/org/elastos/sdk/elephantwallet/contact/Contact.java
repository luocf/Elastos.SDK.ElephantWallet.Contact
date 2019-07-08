package org.elastos.sdk.elephantwallet.contact;

import org.elastos.tools.crosspl.CrossBase;
import org.elastos.tools.crosspl.annotation.CrossClass;
import org.elastos.tools.crosspl.annotation.CrossInterface;

public final class Contact {
    @CrossClass
    public static final class Factory extends CrossBase {
        @CrossInterface
        public static native void SetLogLevel(int level);

        @CrossInterface
        public static native int SetLocalDataDir(String dir);

        @CrossInterface
        public static native Contact Create();
    } // class Factory

}
