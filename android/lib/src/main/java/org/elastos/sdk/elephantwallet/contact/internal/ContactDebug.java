package org.elastos.sdk.elephantwallet.contact.internal;

import org.elastos.tools.crosspl.CrossBase;
import org.elastos.tools.crosspl.annotation.CrossClass;
import org.elastos.tools.crosspl.annotation.CrossInterface;

@CrossClass
public class ContactDebug extends CrossBase {
    protected ContactDebug() {
        super(ContactDebug.class.getName(), 0);
    }

    @CrossInterface
    public static native int GetCachedDidProp(StringBuffer value);
} // class Factory
